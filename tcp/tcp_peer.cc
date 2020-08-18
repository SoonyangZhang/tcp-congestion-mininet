#include <memory.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "dmlc_logging.h"
#include "base/byte_codec.h"
#include "tcp_peer.h"
#include "tcp_server.h"
#include "anet.h"
namespace tcp{
const int kBufferSize=1500;
const int klogInterval=5000;//5s;
int kRcvBufferLen = 2*1024*1024*8;
int kSndBufferLen = 2*1024*1024*8;
const char *done_msg="read all done";
void ReadEventCallback(aeEventLoop *el, int fd, void *arg, int mask){
	TcpPeer *peer=static_cast<TcpPeer*>(arg);
	peer->NotifiRead();
}
TcpPeer::TcpPeer(TcpServer*server,int fd){
	server_=server;
	sockfd_=fd;
	anetNonBlock(nullptr,sockfd_);
    //SetSendBufSize(kSndBufferLen);
    //SetRecvBufSize(kRcvBufferLen);
	aeEventLoop *evb=server_->getEventBase();
    if( aeCreateFileEvent(evb,sockfd_, AE_READABLE,
    		ReadEventCallback, (void*)this) == AE_ERR )
    {
        LOG(INFO)<<"create read event failed";
        Close();
    }
}
void TcpPeer::SetSendBufSize(int len){
    if(sockfd_<0){
        return ;
    }
    int nSndBufferLen =len;
    int nLen          = sizeof(int);
    setsockopt(sockfd_, SOL_SOCKET, SO_SNDBUF, (char*)&nSndBufferLen, nLen);
}
void TcpPeer::SetRecvBufSize(int len){
    if(sockfd_<0){
        return ;
    }
    int nRcvBufferLen =len;
    int nLen          = sizeof(int);
    setsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF, (char*)&nRcvBufferLen, nLen);
}
TcpPeer::~TcpPeer(){
  Close();
}
void TcpPeer::SetTraceRecvFun(TraceReceiveData cb){
	tracRecv_=cb;
}
void TcpPeer::NotifiRead(){
    char line[kBufferSize];
    int res=0;
    int32_t now=server_->getWallTime();
    while(true){
        res = read(sockfd_, line, kBufferSize);
        if( res <= 0 )
        {
        	if(errno == EWOULDBLOCK || errno == EAGAIN){
        		//no data;
        	}else{
            	LOG(INFO)<<"read error";
            	server_->PeerClose(sockfd_);
                Close();
        	}
            break;
        }
    	if(first_packet_){
    		basic::DataReader reader(line,res);
    		uint32_t first,second;
    		reader.ReadUInt32(&first);
    		reader.ReadUInt32(&second);
    		LOG(INFO)<<"recv "<<first<<" "<<second;
    		client_id_=first;
    		totalByte_=second;
    		ReportRecvLength(now);
    		nextTimePrintLog_=now+klogInterval;
    		first_packet_=false;
    	}
    	recvByte_+=res;
    	if(now>=nextTimePrintLog_){
    		ReportRecvLength(now);
    		nextTimePrintLog_=now+klogInterval;
    	}
    	if(recvByte_>=totalByte_){
    	    ReportRecvLength(now);
    	    if(!sendReadDone_){
    	    	SendDoneSignal();
    	    	server_->OnPeerReadDoneMsg();
    	    	sendReadDone_=true;
    	    }
    	}
    }
}
void TcpPeer::Close(){
	if(sockfd_>0){
		close(sockfd_);
		sockfd_=0;
	}
	if(!tracRecv_.IsNull()){
		tracRecv_.Nullify();
	}
}
void TcpPeer::SendDoneSignal(){
	LOG(INFO)<<"read done "<<client_id_;
	char buffer[kBufferSize]={0};
	memset(buffer,0,kBufferSize);
	int msglen=strlen(done_msg);
	memcpy(buffer,done_msg,msglen);
	WriteMessage(buffer, msglen+1);
}
void TcpPeer::ReportRecvLength(uint32_t now){
	if(!tracRecv_.IsNull()){
		tracRecv_(client_id_,now,recvByte_);
	}
}
// ret wirte length
int TcpPeer::WriteMessage(const char *msg, int len){
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
}

