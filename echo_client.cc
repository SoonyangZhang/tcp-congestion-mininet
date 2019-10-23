#include <iostream>
#include <string>
#include <utility>
#include <list>
#include "tcp_client.h"
#include "network_thread.h"
#include "logging.h"
const char *serv_addr="127.0.0.1";
#define SERV_PORT 8888

int main(){
	uint32_t client_id=1;
	uint32_t totalSend=1400*3;
	int i=0;
	int connection=2;
    std::string cc("cubic");
	tcp::NetworkThread loop;
	loop.TriggerTasksLibEvent();
	std::list<std::shared_ptr<tcp::TcpClient>> clients;
	for(i=0;i<connection;i++){
		std::shared_ptr<tcp::TcpClient> client(new tcp::TcpClient(&loop,serv_addr,SERV_PORT,cc));
		client->setSenderInfo(client_id,totalSend);
		client->AsynConnect();
		client_id++;
		clients.push_back(client);
	}
	loop.Dispatch();
	LOG(INFO)<<"end";
	return 0;
}
