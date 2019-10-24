#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include "proto_time.h"
#include "logging.h"
#include "tcp/tcp_server.h"
#include "tcp/tcp_client.h"
#include "cmdline.h"

using namespace tcp;
bool m_running=true;
void signal_exit_handler(int sig)
{
	m_running=false;
}
using namespace std;
class PeerCouner:public tcp::ActiveClientCounter{
public:
	PeerCouner(int32_t clients){
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
int main(int argc, char *argv[]){
	signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGTSTP, signal_exit_handler);
    cmdline::parser a;
    a.add<int>("port", 'p', "port number", false, 80, cmdline::range(1, 65535));
    a.add<string>("log", 'l', "log file", false, "server_log.txt");
    a.add<int>("flows", 'f', "connection", false, 200, cmdline::range(1, 65535));
    a.parse_check(argc, argv);
    int port=a.get<int>("port");
    std::string log=a.get<string>("log");
    int connection=a.get<int>("flows");
    PeerCouner counter(connection);
    TcpServer server(log);
    server.RegisterCounter(&counter);
    server.Bind(port);
    while(m_running){
        server.Loop();
        if(counter.IsAllDeactive()){
            break;
        }
    }
    server.Close();
    LOG(INFO)<<"server stop";
    base::TimeSleep(1000);
    return 0;
}
