#pragma once
#include <event2/event.h>
#include <event2/bufferevent.h>
#include "base/callback.h"
namespace tcp{
class TcpServer;
class TcpPeer{
public:
	TcpPeer(TcpServer*server,evutil_socket_t fd);
	~TcpPeer();
	typedef base::Callback<void,uint32_t,uint32_t,uint32_t> TraceReceiveData;
	void SetTraceRecvFun(TraceReceiveData cb);
	void NotifiError(short event);
	void NotifiRead();
	void BufferFree();
	void Close();
private:
	void NotifiCloseToServer();
	void SendDoneSignal();
	void ReportRecvLength(uint32_t now);
	TcpServer* server_{nullptr};
	evutil_socket_t sockfd_{0};
	struct bufferevent *bev_{nullptr};
	bool first_packet_{true};
	uint32_t client_id_{0};
	uint32_t recvByte_{0};
	uint32_t totalByte_{0};
	TraceReceiveData tracRecv_;
	uint32_t nextTimePrintLog_{0};
};
}
