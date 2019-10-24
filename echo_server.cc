#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include "proto_time.h"
#include "logging.h"
#include "tcp/tcp_server.h"
#include "cmdline.h"

using namespace tcp;
bool m_running=true;
void signal_exit_handler(int sig)
{
	m_running=false;
}
using namespace std;
int main(int argc, char *argv[]){
	signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGTSTP, signal_exit_handler);
    cmdline::parser a;
    a.add<int>("port", 'p', "port number", false, 80, cmdline::range(1, 65535));
    a.add<string>("log", 'l', "log file", true, "server_log.txt");
    a.parse_check(argc, argv);
    int port=a.get<int>("port");
    std::string log=a.get<string>("log");
    TcpServer server(log);
    server.Bind(port);
    while(m_running){
        server.Loop();
    }
    server.Close();
    LOG(INFO)<<"server stop";
    base::TimeSleep(1000);
    return 0;
}
