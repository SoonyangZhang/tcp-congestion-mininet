#include <time.h>
#include <string.h>
#include <unistd.h>
#include "base/logging.h"
#include "tcp_client.h"
#include "network_thread.h"
namespace tcp{
const int MAX_LINE=1024;
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
	LOG(INFO)<<"socket error";
}
TcpClient::TcpClient(NetworkThread* thread,const char*serv_ip,uint16_t port)
:UsedOnce_(TcpClientThread,this){
	thread_=thread;
    if((sockfd_= socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG(INFO)<<"socket\n";
        return;
    }
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
		struct timeval tv;
		struct event_base *evb=thread_->getEventBase();
    	evutil_make_socket_nonblocking(sockfd_);
    	bev_=bufferevent_socket_new(evb, sockfd_, BEV_OPT_CLOSE_ON_FREE);
    	bufferevent_setcb(bev_, ReadEventCallback, NULL, ErrorCallback, (void *)this);
    	bufferevent_enable(bev_, EV_READ | EV_PERSIST);

		event_assign(&write_event_, evb, -1, 0,WriteEventCallback, (void*)this);
		evutil_timerclear(&tv);
		tv.tv_sec = 0;
		event_add(&write_event_, &tv);
	}
}
void TcpClient::NotifiRead(){
    char line[MAX_LINE + 1];
    int n;
    evutil_socket_t fd = bufferevent_getfd(bev_);
    printf("fd = %u\n",fd);
    while((n = bufferevent_read(bev_, line, MAX_LINE)) > 0)
    {
        line[n] = '\0';
        printf("fd = %u, read from server: %s\n", fd, line);
    }
    printf("read = %u\n",n);
}
void TcpClient::NotifiWrite(){
	char msg[MAX_LINE]={"hello world"};
	uint32_t n=strlen(msg)+1;
    	bufferevent_write(bev_, msg, n);
}
void TcpClient::Close(){
	if(sockfd_>0){
		close(sockfd_);
	}
}
void TcpClient::BufferFree(){
	if(bev_){
		bufferevent_free(bev_);
		bev_=nullptr;
	}
}
}
