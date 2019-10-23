#include "tcp_client.h"
#include "network_thread.h"
#include "logging.h"
const char *serv_addr="127.0.0.1";
#define SERV_PORT 9999

int main(){
	tcp::NetworkThread loop;
	loop.TriggerTasksLibEvent();
	tcp::TcpClient client(&loop,serv_addr,SERV_PORT);
	client.AsynConnect();
	loop.Dispatch();
	LOG(INFO)<<"end";
	return 0;
}
