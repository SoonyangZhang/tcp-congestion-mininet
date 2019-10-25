#pragma once
#include <memory>
#include <utility>
#include <map>
#include <list>
#include "ae.h"
#include "proto_time.h"
#include "tcp_trace.h"
namespace tcp{
class TcpPeer;
class ActiveClientCounter;
class TcpServer{
public:
	TcpServer(std::string &name);
	~TcpServer();
	void CreateSocket(uint16_t port);
	void LoopOnce();
	void Accept();
	void PeerClose(int);
	void OnPeerReadDoneMsg();
	aeEventLoop* getEventBase(){
		return evb_;
	}
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
	aeEventLoop *evb_{nullptr};
	int listenfd_{0};
	std::map<int,std::shared_ptr<TcpPeer>> peers_;
	std::list<std::shared_ptr<TcpPeer>> waitForDelele_;
	base::SystemClock clock_;
	base::ProtoTime startTime_{base::ProtoTime::Zero()};
	std::shared_ptr<TcpTrace> tracer_;
	ActiveClientCounter *counter_{nullptr};
};
}
