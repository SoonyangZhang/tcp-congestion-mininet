#include <iostream>
#include <signal.h>
#include <string>
#include <utility>
#include <list>
#include <memory.h>
#include "cmdline.h"
#include "proto_time.h"
#include "tcp_client.h"
#include "network_thread.h"
#include "logging.h"
bool m_running=true;
void signal_exit_handler(int sig)
{
	m_running=false;
}
class ClientCouner:public tcp::ActiveClientCounter{
public:
	ClientCouner(int32_t clients){
		clients_=clients;
	}
	void Decrease() override{
		clients_--;
	}
	bool IsAllDeactive(){
		bool ret=false;
		if(clients_<=0){
			ret=true;
		}
		return ret;
	}
private:
	int clients_;
};
using namespace std;
int kRcvBufferLen = 1024*1024*8;
int kSndBufferLen = 2*1024*1024*8;
int main(int argc, char *argv[]){
	signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGTSTP, signal_exit_handler);
    cmdline::parser a;
    a.add<string>("host", 'h', "host name", false, "127.0.0.1");
    a.add<string>("local", 'l', "local", false, "127.0.0.1");
    a.add<string>("congestion", 'g', "congestion algo", false, "cubic");
    a.add<int>("port", 'p', "port number", false, 80, cmdline::range(1, 65535));
    a.add<int>("clientid", 'c', "client id", false, 1, cmdline::range(1, 65535));
    a.add<int>("flows", 'f', "connection", false, 1, cmdline::range(1, 65535));
    a.parse_check(argc, argv);
    std::string host=a.get<string>("host");
    std::string local=a.get<string>("local");
    std::string cc=a.get<string>("congestion");
    int port=a.get<int>("port");
    int client_id=a.get<int>("clientid");
    int connection=a.get<int>("flows");
    char serv_addr[32]={0};
    char client_addr[32]={0};
    memcpy(serv_addr,host.c_str(),host.size());
    memcpy(client_addr,local.c_str(),local.size());
	uint32_t totalSend=60*1024*1024;
    
	int i=0;
	ClientCouner counter(connection);
	tcp::NetworkThread loop;
	std::list<std::shared_ptr<tcp::TcpClient>> clients;
	for(i=0;i<connection;i++){
		std::shared_ptr<tcp::TcpClient> client(new tcp::TcpClient(&loop,&counter,serv_addr,port,cc));
		client->Bind(client_addr);
        //client->SetSendBufSize(kSndBufferLen);
       // client->SetRecvBufSize(kRcvBufferLen);
		client->SetSenderInfo(client_id,totalSend);
		client->AsynConnect();
		client_id++;
		clients.push_back(client);
	}
	while(m_running){

		loop.LoopOnce();
		if(counter.IsAllDeactive()){
			break;
		}
	}
	return 0;
}
