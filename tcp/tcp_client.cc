#include <time.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <algorithm>
#include "base/byte_codec.h"
#include "base/logging.h"
#include "tcp_client.h"
#include "network_thread.h"
#include "logging.h"
namespace tcp{
const int MAX_LINE=1400;
const int kPacketBatchSize=10;
static const size_t TCP_CC_NAME_MAX = 16;
void TcpClientThread(void *arg){
	TcpClient *client=static_cast<TcpClient*>(arg);
	client->SynConnect();
}
void WriteEventCallback(evutil_socket_t fd, short event, void *arg){
	TcpClient *client=static_cast<TcpClient*>(arg);
	client->NotifiWrite();
}
void ReadEventCallback(struct bufferevent *bev, void *arg){
	TcpClient *client=static_cast<TcpClient*>(arg);
	client->NotifiRead();
}
void ErrorCallback(struct bufferevent *bev, short event, void *arg){
	TcpClient *client=static_cast<TcpClient*>(arg);
	client->NotifiError(event);
}
TcpClient::TcpClient(NetworkThread* thread,const char*serv_ip,
		uint16_t port,std::string &cc_algo)
:UsedOnce_(TcpClientThread,this){
	thread_=thread;
    if((sockfd_= socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG(INFO)<<"socket\n";
        return;
    }
    setCongestionAlgo(cc_algo);
    bzero(&servaddr_, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_port = htons(port);
    if(inet_pton(AF_INET, serv_ip, &servaddr_.sin_addr) < 1)
    {
    	LOG(INFO)<<"inet_ntop\n";
        return ;
    }
    LOG(INFO)<<"fd "<<sockfd_;
}
TcpClient::~TcpClient(){
	Close();
	BufferFree();
}
void TcpClient::setSenderInfo(uint32_t cid,uint32_t length){
	client_id_=cid;
	if(length<MAX_LINE){
		length=MAX_LINE;
	}
	totalByte_=length;
}
void TcpClient::AsynConnect(){
	if(sockfd_>0){
		asyconnect_=true;
		UsedOnce_.Start();
	}
}
void TcpClient::SynConnect(){
	struct event_base *evb=thread_->getEventBase();
    if(connect(sockfd_, (struct sockaddr *) &servaddr_, sizeof(servaddr_)) < 0)
    {
    	LOG(INFO)<<"connect error";
    }else{
    	connected_=true;
    	LOG(INFO)<<"connect success";
    }
	if(asyconnect_){
		thread_->PostTask([this](){
			StopUseOnce();
		});
	}
	thread_->PostTask([this](){
		NotifiConnect();
	});
}

void TcpClient::StopUseOnce(){
	UsedOnce_.Stop();
}
void TcpClient::NotifiConnect(){
	if(connected_){
		random_.seedTime();
		struct event_base *evb=thread_->getEventBase();
    	evutil_make_socket_nonblocking(sockfd_);
    	bev_=bufferevent_socket_new(evb, sockfd_, BEV_OPT_CLOSE_ON_FREE);
    	bufferevent_setcb(bev_, ReadEventCallback, NULL, ErrorCallback, (void *)this);
    	bufferevent_enable(bev_, EV_READ | EV_PERSIST);
    	NextWriteEvent(0);
	}
}
void TcpClient::NotifiRead(){
    char line[MAX_LINE + 1];
    int n;
    while((n = bufferevent_read(bev_, line, MAX_LINE)) > 0)
    {
        line[n] = '\0';
        if(!recv_ack_){
        	recv_ack_=true;
        }
    }
    if(recv_ack_){
    	LOG(INFO)<<"trans success";
    	Close();
    }
}
void TcpClient::NotifiWrite(){
	if(sockfd_<=0){
		return;
	}
	char msg[MAX_LINE]={0};
	int ret=0;
	if(!first_sent_){
		basic::DataWriter writer(msg,MAX_LINE);
		writer.WriteUInt32(client_id_);
		writer.WriteUInt32(totalByte_);
	    ret=bufferevent_write(bev_, msg, MAX_LINE);
	    if(ret==0){
	    	IncreaseWriteBytes(MAX_LINE);
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
	if(sendByte_<totalByte_){
		uint32_t millis=random_.nextInt(0,100);
		NextWriteEvent(millis);
	}
}
void TcpClient::NotifiError(short event){
    if (event & BEV_EVENT_TIMEOUT) {
        printf("Timed out\n");
    }
    else if (event & BEV_EVENT_EOF) {
        printf("connection closed\n");
        Close();
    }
    else if (event & BEV_EVENT_ERROR) {
        printf("some other error\n");
        Close();
    }
}
void TcpClient::NextWriteEvent(int millis){
	struct timeval tv;
	struct event_base *evb=thread_->getEventBase();
	event_assign(&write_event_, evb, -1, 0,WriteEventCallback, (void*)this);
	evutil_timerclear(&tv);
	tv.tv_sec = millis/1000;
	tv.tv_usec=(millis%1000)*1000;
	event_add(&write_event_, &tv);
}
void TcpClient::Close(){
	if(sockfd_>0){
		evutil_closesocket(sockfd_);
		sockfd_=0;
	}
}
void TcpClient::BufferFree(){
	if(bev_){
		bufferevent_free(bev_);
		bev_=nullptr;
	}
}
void TcpClient::setCongestionAlgo(std::string &cc_algo){
	cc_algo_=cc_algo;
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
	    ret=bufferevent_write(bev_, msg, write);
	    if(ret==0){
	    	total+=write;
	    	remain-=write;
	    }else{
	    	LOG(INFO)<<client_id_<<" write error";
	    	break;
	    }
	}
	return total;
}
}
