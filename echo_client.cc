#include <iostream>
#include <signal.h>
#include <string>
#include <utility>
#include <list>
#include "base/proto_time.h"
#include "tcp_client.h"
#include "network_thread.h"
#include "logging.h"
const char *serv_addr="10.0.5.2";
#define SERV_PORT 8888
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
int main(){
	signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGTSTP, signal_exit_handler);
	uint32_t client_id=1;
	uint32_t totalSend=100*1024*1024;
	int i=0;
	int connection=2;
	ClientCouner counter(connection);
    std::string cc("cubic");
	tcp::NetworkThread loop;
	loop.TriggerTasksLibEvent();
	std::list<std::shared_ptr<tcp::TcpClient>> clients;
	for(i=0;i<connection;i++){
		std::shared_ptr<tcp::TcpClient> client(new tcp::TcpClient(&loop,&counter,serv_addr,SERV_PORT,cc));
		client->setSenderInfo(client_id,totalSend);
		client->AsynConnect();
		client_id++;
		clients.push_back(client);
	}
	while(m_running){
		loop.Loop();
		if(counter.IsAllDeactive()){
			break;
		}
	}

	LOG(INFO)<<"end";
	return 0;
}
