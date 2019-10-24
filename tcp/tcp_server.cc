#include "tcp_server.h"
#include "tcp_peer.h"
#include "logging.h"
#include "tcp_client.h"
namespace tcp{
const int kListenBacklog=128;
void AcceptEventCallback(evutil_socket_t listener, short event, void *arg){
	TcpServer *server=static_cast<TcpServer*>(arg);
	server->Accept();
}
void DeleteEventCallback(evutil_socket_t fd, short event, void *arg){
	TcpServer *server=static_cast<TcpServer*>(arg);
	server->DeletePeerList();
}
TcpServer::TcpServer(std::string &name){
	startTime_=clock_.Now();
	tracer_.reset(new TcpTrace(name));
}
TcpServer::~TcpServer(){
	if(evb_){
		event_base_free(evb_);
		evb_=nullptr;
	}
	Close();
}
void TcpServer::Bind(uint16_t port){
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(listenfd_ > 0);
    evutil_make_listen_socket_reuseable(listenfd_);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);
    if (bind(listenfd_, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return;
    }
    Listen();
}
void TcpServer::Accept(){
    evutil_socket_t fd;
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
void TcpServer::PeerClose(evutil_socket_t fd){
	auto it=peers_.find(fd);
	if(it!=peers_.end()){
		std::shared_ptr<TcpPeer> peer=it->second;
		waitForDelele_.push_back(peer);
		peers_.erase(it);
		TriggerDelete();
	}
}
void TcpServer::OnPeerReadDoneMsg(){
	if(counter_){
		counter_->Decrease();
	}
}
void TcpServer::Loop(){
	if(running_){
		event_base_loop(evb_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}
}
void TcpServer::Stop(){
	running_=false;
}
void TcpServer::Listen(){
    if (listen(listenfd_, kListenBacklog) < 0) {
        perror("listen");
        return;
    }
    printf ("Listening...\n");
    evutil_make_socket_nonblocking(listenfd_);
    evb_= event_base_new();
    event_assign(&listen_event_, evb_,listenfd_,EV_READ|EV_PERSIST,AcceptEventCallback, (void*)this);
    event_add(&listen_event_, NULL);
}
void TcpServer::TriggerDelete(){
	struct timeval tv;
	event_assign(&delete_event_, evb_, -1, 0,DeleteEventCallback, (void*)this);
	evutil_timerclear(&tv);
	tv.tv_sec =0;
	event_add(&delete_event_, &tv);
}
void TcpServer::DeletePeerList(){
	while(!waitForDelele_.empty()){
		auto it=waitForDelele_.begin();
		waitForDelele_.erase(it);
	}
}
void TcpServer::Close(){
	if(listenfd_>0){
		evutil_closesocket(listenfd_);
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
