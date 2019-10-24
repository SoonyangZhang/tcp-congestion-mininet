#include <memory.h>
#include <string.h>
#include "logging.h"
#include "base/byte_codec.h"
#include "tcp_peer.h"
#include "tcp_server.h"

namespace tcp{
const int kBufferSize=1500;
const int klogInterval=5000;//5s;
int kRcvBufferLen = 2*1024*1024*8;
int kSndBufferLen = 2*1024*1024*8;
const char *done_msg="read all done";
void ReadEventCallback(struct bufferevent *bev, void *arg){
	TcpPeer *peer=static_cast<TcpPeer*>(arg);
	peer->NotifiRead();
}
void ErrorCallback(struct bufferevent *bev, short event, void *arg){
	TcpPeer *peer=static_cast<TcpPeer*>(arg);
	peer->NotifiError(event);
}
TcpPeer::TcpPeer(TcpServer*server,evutil_socket_t fd){
	server_=server;
	sockfd_=fd;
	evutil_make_socket_nonblocking(sockfd_);
    SetSendBufSize(kSndBufferLen);
    SetRecvBufSize(kRcvBufferLen);
	struct event_base *evb=server_->getEventBase();
	bev_= bufferevent_socket_new(evb,sockfd_, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev_, ReadEventCallback, NULL, ErrorCallback, (void*)this);
    bufferevent_enable(bev_, EV_READ|EV_WRITE|EV_PERSIST);
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
	BufferFree();
}
void TcpPeer::SetTraceRecvFun(TraceReceiveData cb){
	tracRecv_=cb;
}
void TcpPeer::NotifiError(short event){
    if (event & BEV_EVENT_TIMEOUT) {
        printf("Timed out\n");
    }
    else if (event & BEV_EVENT_EOF) {
        printf("connection closed\n");
        server_->PeerClose(sockfd_);
        Close();
    }
    else if (event & BEV_EVENT_ERROR) {
        printf("some other error\n");
        server_->PeerClose(sockfd_);
        Close();
    }
}
void TcpPeer::NotifiRead(){
    char line[kBufferSize];
    int n=0;
    int32_t now=server_->getWallTime();
    while (n = bufferevent_read(bev_, line, kBufferSize), n > 0) {
    	if(first_packet_){
    		basic::DataReader reader(line,n);
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
    	recvByte_+=n;
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
void TcpPeer::BufferFree(){
	if(bev_){
		bufferevent_free(bev_);
		bev_=nullptr;
	}
}
void TcpPeer::Close(){
	if(sockfd_>0){
		evutil_closesocket(sockfd_);
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
	bufferevent_write(bev_, buffer, msglen+1);
}
void TcpPeer::ReportRecvLength(uint32_t now){
	if(!tracRecv_.IsNull()){
		tracRecv_(client_id_,now,recvByte_);
	}
}
}

