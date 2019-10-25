#pragma once
#include "ae.h"
#include "base/callback.h"
namespace tcp{
class TcpServer;
class TcpPeer{
public:
	TcpPeer(TcpServer*server,int fd);
	~TcpPeer();
    void SetSendBufSize(int len);
    void SetRecvBufSize(int len);
	typedef base::Callback<void,uint32_t,uint32_t,uint32_t> TraceReceiveData;
	void SetTraceRecvFun(TraceReceiveData cb);
	void NotifiError(short event);
	void NotifiRead();
	void Close();
private:
	void NotifiCloseToServer();
	void SendDoneSignal();
	void ReportRecvLength(uint32_t now);
	int WriteMessage(const char *msg, int len);
	TcpServer* server_{nullptr};
	int sockfd_{0};
	bool first_packet_{true};
	uint32_t client_id_{0};
	uint32_t recvByte_{0};
	uint32_t totalByte_{0};
	TraceReceiveData tracRecv_;
	uint32_t nextTimePrintLog_{0};
	bool sendReadDone_{false};
};
}
