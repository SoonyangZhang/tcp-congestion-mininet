#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include "proto_time.h"
#include "base/logging.h"
#include "tcp/tcp_server.h"
#define LISTEN_PORT 8888

using namespace tcp;
bool m_running=true;
void signal_exit_handler(int sig)
{
	m_running=false;
}
int main(){
	signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGTSTP, signal_exit_handler);
    TcpServer server;
    server.Bind(LISTEN_PORT);
    while(m_running){
        server.Loop();
    }
    server.Close();
    LOG(INFO)<<"server stop";
    base::TimeSleep(1000);
    return 0;
}
