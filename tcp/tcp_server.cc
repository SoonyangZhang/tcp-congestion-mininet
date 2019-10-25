#include <unistd.h>
#include "tcp_server.h"
#include "tcp_peer.h"
#include "logging.h"
#include "tcp_client.h"
#include "anet.h"
namespace tcp{
const int kListenBacklog=128;
void AcceptTcpHandler(aeEventLoop *el, int fd, void *arg, int mask){
	TcpServer *server=static_cast<TcpServer*>(arg);
	server->Accept();
}
int DeletePeerTimer(aeEventLoop *el, long long id, void *arg){
	TcpServer *server=static_cast<TcpServer*>(arg);
	server->DeletePeerList();
	return 0;
}
TcpServer::TcpServer(std::string &name){
	startTime_=clock_.Now();
	tracer_.reset(new TcpTrace(name));
}
TcpServer::~TcpServer(){
	if(evb_){
		aeDeleteEventLoop(evb_);
		evb_=nullptr;
	}
	Close();
}
void TcpServer::CreateSocket(uint16_t port){
	char *bind_ip="0.0.0.0";
	listenfd_= anetTcpServer(nullptr,port, bind_ip,kListenBacklog);
    CHECK(listenfd_ > 0);
    anetNonBlock(nullptr,listenfd_);
    printf("Listening...\n");
    evb_=aeCreateEventLoop(1024*10);
    if( aeCreateFileEvent(evb_,listenfd_, AE_READABLE,
        AcceptTcpHandler, (void*)this) == AE_ERR ){
    	LOG(INFO)<<"listen error";
    	return ;
    }
}
void TcpServer::Accept(){
    int fd;
    struct sockaddr_in sin;
    socklen_t slen = sizeof(sin);
    fd = accept(listenfd_, (struct sockaddr *)&sin, &slen);
    if (fd < 0) {
        LOG(ERROR)<<"accept error";
        return;
    }
    if (fd > FD_SETSIZE) {
    	LOG(ERROR)<<"fd > FD_SETSIZE";
        return;
    }
    std::shared_ptr<TcpPeer> peer(new TcpPeer(this,fd));
    peer->SetTraceRecvFun(base::MakeCallback(&TcpServer::OnTraceData,this));
    peers_.insert(std::make_pair(fd,peer));
}
void TcpServer::PeerClose(int fd){
	auto it=peers_.find(fd);
	if(it!=peers_.end()){
		std::shared_ptr<TcpPeer> peer=it->second;
		waitForDelele_.push_back(peer);
		peers_.erase(it);
		aeCreateTimeEvent(evb_, 1, DeletePeerTimer, (void*)this, NULL);
	}
}
void TcpServer::OnPeerReadDoneMsg(){
	if(counter_){
		counter_->Decrease();
	}
}
void TcpServer::LoopOnce(){
	if(evb_){
		aeLoopOnce(evb_);
	}
}
void TcpServer::DeletePeerList(){
	while(!waitForDelele_.empty()){
		auto it=waitForDelele_.begin();
		waitForDelele_.erase(it);
	}
}
void TcpServer::Close(){
	if(listenfd_>0){
		close(listenfd_);
		listenfd_=0;
	}
}
int64_t TcpServer::getWallTime(){
	base::ProtoTime now=clock_.Now();
	base::TimeDelta delta=now-startTime_;
	int64_t time=delta.ToMilliseconds();
	return time;
}
void TcpServer::OnTraceData(uint32_t id,uint32_t ts,uint32_t len){
	if(tracer_){
		tracer_->OnRecvData(id,ts,len);
	}
}
}
