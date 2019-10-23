#pragma once
#include <memory>
#include <utility>
#include <map>
#include <list>
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
namespace tcp{
class TcpPeer;
class TcpServer{
public:
	~TcpServer();
	void Bind(uint16_t port);
	void Loop();
	void Stop();
	void Accept();
	void PeerClose(evutil_socket_t);
	struct event_base* getEventBase(){
		return evb_;
	}
	void TriggerDelete();
	void DeletePeerList();
	void Close();
private:
	void Listen();
	bool running_{true};
	struct event_base *evb_{nullptr};
	struct event listen_event_;
	struct event delete_event_;
	evutil_socket_t listenfd_{0};
	std::map<evutil_socket_t,std::shared_ptr<TcpPeer>> peers_;
	std::list<std::shared_ptr<TcpPeer>> waitForDelele_;
};
}
