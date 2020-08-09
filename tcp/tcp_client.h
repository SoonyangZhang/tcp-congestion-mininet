#pragma once
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "base/base_thread.h"
#include "base/random.h"
#include <fstream>
namespace tcp{
class NetworkThread;
class ActiveClientCounter{
public:
	virtual void Decrease()=0;
	virtual ~ActiveClientCounter(){}
};
class TcpClient{
public:
	TcpClient(NetworkThread* thread,ActiveClientCounter *counter,const char*serv_ip,
			uint16_t port,std::string &cc_algo);
	~TcpClient();
	void Bind(const char *local_ip);
    void SetSendBufSize(int len);
    void SetRecvBufSize(int len);
	void SetSenderInfo(uint32_t cid,uint32_t length);
	void AsynConnect();
	void SynConnect();
	void NotifiConnect();
	void StopUseOnce();
	void NotifiRead();
	int NextWriteTime();
	void Close();
	bool IsBulkReceivedByPeer(){return recv_ack_;}
    void LogTcpInfoEvent();
private:
	void SetCongestionAlgo();
	void IncreaseWriteBytes(int size);
	int GetBatchSize();
	int WritePacketInBatch(int length);
    void StopConectionThread();
    void NotifiDeactiveMsg();
    void DeleteReadEvent();
    int WriteMessage(const char *msg, int len);
    void UpdateTcpInfo();
	uint32_t client_id_{0};
	NetworkThread *thread_;
	ActiveClientCounter *counter_{nullptr};
	int sockfd_{-1};
	struct sockaddr_in servaddr_;
	base::BaseThread UsedOnce_;
	bool asyconnect_{false};
	bool connected_{false};
	base::Random random_;
	bool first_sent_{false};
	bool recv_ack_{false};
	uint32_t sendByte_{0};
	uint32_t totalByte_{0};
	std::string cc_algo_{"cubic"};
	bool deactive_sent_{false};
	bool readEventRegisted_{false};
    uint64_t tcp_bytes_sent_=0;
        uint32_t min_rtt_=0;
        uint32_t rtt_=0;
    uint32_t last_log=0;
    std::fstream log_;
};
}
