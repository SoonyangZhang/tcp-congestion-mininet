#include "tcp/tcp_trace.h"

using namespace tcp;
int main(){
    std::string name("time_log.txt");
    TcpTrace trace(name);
    trace.OnRecvData(1,0,1234);
    return 0;
}