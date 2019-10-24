#pragma once
#include <memory>
#include <utility>
#include <map>
#include <list>
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include "base/proto_time.h"
#include "tcp_trace.h"
namespace tcp{
class TcpPeer;
class ActiveClientCounter;
class TcpServer{
public:
	TcpServer(std::string &name);
	~TcpServer();
	void Bind(uint16_t port);
	void Loop();
	void Stop();
	void Accept();
	void PeerClose(evutil_socket_t);
	void OnPeerReadDoneMsg();
	struct event_base* getEventBase(){
		return evb_;
	}
	void TriggerDelete();
	void DeletePeerList();
	void Close();
	int64_t getWallTime();
	void OnTraceData(uint32_t id,uint32_t ts,uint32_t len);
	void RegisterCounter(ActiveClientCounter *counter){
		counter_=counter;
	}
private:
	void Listen();
	bool running_{true};
	struct event_base *evb_{nullptr};
	struct event listen_event_;
	struct event delete_event_;
	evutil_socket_t listenfd_{0};
	std::map<evutil_socket_t,std::shared_ptr<TcpPeer>> peers_;
	std::list<std::shared_ptr<TcpPeer>> waitForDelele_;
	base::SystemClock clock_;
	base::ProtoTime startTime_{base::ProtoTime::Zero()};
	std::shared_ptr<TcpTrace> tracer_;
	ActiveClientCounter *counter_{nullptr};
};
}
