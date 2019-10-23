#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/util.h>
#include "base/base_thread.h"
namespace tcp{
class NetworkThread;
class TcpClient{
public:
	TcpClient(NetworkThread* thread,const char*serv_ip,uint16_t port);
	~TcpClient();
	//fuck the io blocking of conect,so thread is need;
	void AsynConnect();
	void SynConnect();
	void NotifiConnect();
	void StopUseOnce();
	void NotifiRead();
	void NotifiWrite();
	void Close();
	void BufferFree();
private:
	NetworkThread *thread_;
	int sockfd_{-1};
	struct sockaddr_in servaddr_;
	base::BaseThread UsedOnce_;
	struct event write_event_;
	struct bufferevent *bev_{nullptr};
	bool asyconnect_{false};
	bool connected_{false};
};
}
