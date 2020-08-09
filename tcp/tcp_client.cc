#include <time.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <algorithm>
#include "byte_codec.h"
#include "logging.h"
#include "anet.h"
#include "tcp_client.h"
#include "network_thread.h"
#include "logging.h"
#include "tcp_info.h"
namespace tcp{
const int MAX_LINE=1400;
const int kPacketBatchSize=10;
static const size_t TCP_CC_NAME_MAX = 16;
uint32_t log_interval=5000;
void TcpClientThread(void *arg){
	TcpClient *client=static_cast<TcpClient*>(arg);
	client->SynConnect();
}
int WriteEventTimer(aeEventLoop *el, long long id, void *arg){
	TcpClient *client=static_cast<TcpClient*>(arg);
	return client->NextWriteTime();
}
void ReadEventCallback(aeEventLoop *el, int fd, void *arg, int mask){
	TcpClient *client=static_cast<TcpClient*>(arg);
	client->NotifiRead();
}
TcpClient::TcpClient(NetworkThread* thread,ActiveClientCounter *counter,const char*serv_ip,
		uint16_t port,std::string &cc_algo)
:UsedOnce_(TcpClientThread,this){
	thread_=thread;
	counter_=counter;
	cc_algo_=cc_algo;
    bzero(&servaddr_, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_port = htons(port);
    if(inet_pton(AF_INET, serv_ip, &servaddr_.sin_addr) < 1)
    {
    	LOG(INFO)<<"inet_ntop\n";
        return ;
    }
}
TcpClient::~TcpClient(){
	Close();
}
void TcpClient::Bind(const char *local_ip){
    if((sockfd_= socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG(INFO)<<"socket\n";
        return;
    }
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = inet_addr("192.168.1.100");
    localaddr.sin_port = 0;  // Any local port will do
    if(inet_pton(AF_INET, local_ip, &localaddr.sin_addr) < 1)
    {
    	LOG(INFO)<<"inet_ntop\n";
        return ;
    }
    bind(sockfd_, (struct sockaddr *)&localaddr, sizeof(localaddr));
	SetCongestionAlgo();
}
void TcpClient::SetSendBufSize(int len){
    if(sockfd_<0){
        return ;
    }
    int nSndBufferLen =len;
    int nLen          = sizeof(int);
    setsockopt(sockfd_, SOL_SOCKET, SO_SNDBUF, (char*)&nSndBufferLen, nLen);
}
void TcpClient::SetRecvBufSize(int len){
    if(sockfd_<0){
        return ;
    }
    int nRcvBufferLen =len;
    int nLen          = sizeof(int);
    setsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF, (char*)&nRcvBufferLen, nLen);
}
void TcpClient::SetSenderInfo(uint32_t cid,uint32_t length){
	client_id_=cid;
	if(length<MAX_LINE){
		length=MAX_LINE;
	}
	totalByte_=length;
    std::string name=cc_algo_+"_"+std::to_string(client_id_)+".txt";
    log_.open(name.c_str(), std::fstream::out);
}
void TcpClient::AsynConnect(){
	if(sockfd_>0){
		asyconnect_=true;
		UsedOnce_.Start();
	}
}
void TcpClient::SynConnect(){
	if(sockfd_<0){
		LOG(INFO)<<"Call Bind First";
        StopConectionThread();
		return ;
	}
    if(connect(sockfd_, (struct sockaddr *) &servaddr_, sizeof(servaddr_)) < 0)
    {
    	LOG(INFO)<<"connect error";
    }else{
    	connected_=true;
    	LOG(INFO)<<"connect success";
    }
    thread_->PostTask([this](){
		NotifiConnect();
	});
    StopConectionThread();
}
void TcpClient::StopConectionThread(){
	if(asyconnect_){
        asyconnect_=false;
		thread_->PostTask([this](){
			StopUseOnce();
		});
	}
}
void TcpClient::StopUseOnce(){
	UsedOnce_.Stop();
}
void TcpClient::NotifiConnect(){
	if(connected_){
		random_.seedTime();
		aeEventLoop *evb=thread_->getEventBase();
		anetNonBlock(nullptr,sockfd_);
		aeCreateTimeEvent(evb, 1, WriteEventTimer,(void*)this, NULL);
	    if( aeCreateFileEvent(evb,sockfd_, AE_READABLE,
	    		ReadEventCallback, (void*)this) == AE_ERR )
	    {
	        LOG(INFO)<<"create read event failed";
	        close(sockfd_);
	    }
	}
   LogTcpInfoEvent();
}
void TcpClient::NotifiRead(){
    char buffer[MAX_LINE]={0};
    int res;
    while(true){
        res = read(sockfd_, buffer, MAX_LINE);
        if( res <= 0 )
        {
        	if(errno == EWOULDBLOCK || errno == EAGAIN){
        	}else{
            	LOG(INFO)<<"read error";
                Close();
        	}
            break;
        }
        if(!recv_ack_){
        	recv_ack_=true;
        	printf("%d read %s",client_id_,buffer);
        }
        if(recv_ack_){
            LOG(INFO)<<"trans success";
            NotifiDeactiveMsg();
        }
    }
}
int TcpClient::NextWriteTime(){
	if(sockfd_<=0){
		return 0;
	}
	if(GetBatchSize()==0){
		return 0;
	}
	char msg[MAX_LINE]={0};
	int ret=0;
	if(!first_sent_){
		basic::DataWriter writer(msg,MAX_LINE);
		writer.WriteUInt32(client_id_);
		writer.WriteUInt32(totalByte_);
	    ret=WriteMessage(msg, MAX_LINE);
	    CHECK(ret==MAX_LINE);
	    if(ret>0){
	    	IncreaseWriteBytes(ret);
	    }
		first_sent_=true;
		int batch=GetBatchSize();
	    int write=WritePacketInBatch(batch);
	    IncreaseWriteBytes(write);
	}else{
		int batch=GetBatchSize();
	    int write=WritePacketInBatch(batch);
	    IncreaseWriteBytes(write);
	}
	int NextTimems=random_.nextInt(0,10);
	return NextTimems;
}
void TcpClient::Close(){
	if(sockfd_>0){
		DeleteReadEvent();
		UpdateTcpInfo();
		close(sockfd_);
		sockfd_=0;
	}
}
void TcpClient::SetCongestionAlgo(){
    char optval[TCP_CC_NAME_MAX]={0};
    memset(optval,0,TCP_CC_NAME_MAX);
    int copy=std::min(TCP_CC_NAME_MAX,cc_algo_.size());
    strncpy(optval,cc_algo_.c_str(),copy);
    int length=strlen(optval)+1;
    LOG(INFO)<<"cc "<<sockfd_<<" "<<optval;
    int rc=setsockopt(sockfd_,IPPROTO_TCP, TCP_CONGESTION, (void*)optval,length);
    if(rc!=0){
	LOG(INFO)<<cc_algo_<<" is not support";
	}
}
void TcpClient::IncreaseWriteBytes(int size){
	sendByte_+=size;
}
int TcpClient::GetBatchSize(){
	int max_batch=kPacketBatchSize*MAX_LINE;
	int remain=totalByte_-sendByte_;
	int batch=std::min(max_batch,remain);
	return batch;
}
int TcpClient::WritePacketInBatch(int length){
	char msg[MAX_LINE]={0};
	int total=0;
	int remain=length;
	while(remain>0){
		int write=std::min(remain,MAX_LINE);
		int ret=0;
	    ret=WriteMessage(msg, write);
	    if(ret>0){
	    	total+=ret;
	    	remain-=ret;
	    }else{
	    	//LOG(INFO)<<client_id_<<" write error";
	    	break;
	    }
	}
	return total;
}
void TcpClient::NotifiDeactiveMsg(){
	if(!deactive_sent_){
    	if(counter_){
    		counter_->Decrease();
    	}
		deactive_sent_=true;
		LogTcpInfoEvent();
	}
}
void TcpClient::DeleteReadEvent(){
	if(readEventRegisted_){
		aeEventLoop *evb_=thread_->getEventBase();
		if(evb_&&(sockfd_>0)){
			 aeDeleteFileEvent(evb_, sockfd_, AE_READABLE);
		}
	}
}
//http://liubigbin.github.io/2016/04/07/socket%E9%9D%9E%E9%98%BB%E5%A1%9E%E8%AF%BB%E5%86%99/
int TcpClient::WriteMessage(const char *msg, int len){
	int written_bytes=0;
	if(sockfd_<=0){
		return written_bytes;
	}
	written_bytes=write(sockfd_,msg,len);
    if(written_bytes<=0){
        if(errno == EWOULDBLOCK || errno == EAGAIN){
		written_bytes=0;
        }else{
	    written_bytes=0;
            LOG(INFO)<<"write error "<<errno;
            CHECK(0);
        }
    }
	return written_bytes;
}
void TcpClient::LogTcpInfoEvent(){
    UpdateTcpInfo();   
    if(deactive_sent_){
        
    }else{
		thread_->PostDelayedTask([this](){
			LogTcpInfoEvent();
		},log_interval);        
    }
}
void TcpClient::UpdateTcpInfo(){
    if(sockfd_<0){
        return ;
    }
    struct tcp_info_copy info;
    int length=sizeof(struct tcp_info_copy);
    if(getsockopt(sockfd_,IPPROTO_TCP,TCP_INFO,(void*)&info,(socklen_t*)&length)==0){
        tcp_bytes_sent_=info.tcpi_bytes_sent;
        min_rtt_=info.tcpi_min_rtt;
        rtt_=info.tcpi_rtt;
    }  
	if(log_.is_open()){
		char line [256];
		memset(line,0,256);
		float loss=0.0;
                if(tcp_bytes_sent_>totalByte_){
			loss=100.0*(tcp_bytes_sent_-totalByte_)/tcp_bytes_sent_;
		}
		sprintf (line, "%d %16d %16d %16f %llu",
				client_id_,rtt_,min_rtt_,loss,tcp_bytes_sent_);
		log_<<line<<std::endl;
	}    
}
}
