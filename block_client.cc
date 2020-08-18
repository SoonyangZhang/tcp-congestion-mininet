/*
https://blog.csdn.net/shine_journey/article/details/72620393
*/
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string>
#include <memory>
#include <algorithm>
#include "dmlc_logging.h"
#include "base/byte_codec.h"
#include "cmdline.h"
using namespace std;
const int kBufferLength=1400;
bool m_running=true;
void signal_exit_handler(int sig)
{
	m_running=false;
}
int main(int argc, char *argv[])
{
    signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGTSTP, signal_exit_handler);
    cmdline::parser a;
    a.add<string>("host", 'h', "host name", false, "127.0.0.1");
    a.add<string>("local", 'l', "local", false, "0.0.0.0");
    a.add<int>("port", 'p', "port number", false, 3333, cmdline::range(1, 65535));
    a.parse_check(argc, argv);
    std::string host=a.get<string>("host");
    std::string local=a.get<string>("local");
    int port=a.get<int>("port");
    char serv_ip[32]={0};
    memcpy(serv_ip,host.c_str(),host.size());
    char client_ip[32]={0};
    memcpy(client_ip,local.c_str(),local.size());
    int client_id=12;
    int totalSend=100*1024*1024;
    int byteRemain=totalSend;
    bool first_packet=true;
    char recvBuff[kBufferLength] = {0};
    int sockfd = 0, n = 0;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = inet_addr("192.168.1.100");
    localaddr.sin_port = 0;
    if(inet_pton(AF_INET, client_ip, &localaddr.sin_addr) < 1)
    {
    	LOG(INFO)<<"inet_ntop\n";
        return 0;
    }
    bind(sockfd, (struct sockaddr *)&localaddr, sizeof(localaddr));
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
 
    //将 ip地址 转换
    if (inet_pton(AF_INET,serv_ip, &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }
 
    //向服务端申请建立连接
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }else{
       printf("connected\n"); 
    }
    char sendBuff[kBufferLength] = {0};
    while(m_running){
        if(byteRemain==0){
            break;
        }
        uint32_t sendSize=std::min(kBufferLength,byteRemain);
        if(first_packet){
            basic::DataWriter writer(sendBuff,kBufferLength);
            writer.WriteUInt32(client_id);
            writer.WriteUInt32(totalSend);
            first_packet=false;
        }
        int ret=write(sockfd, sendBuff,sendSize);
        byteRemain-=ret;
    }
    LOG(INFO)<<"write done";
    while (1)
    {
        if ((n = read(sockfd, recvBuff, kBufferLength)) > 0) {
            printf("read %s\n",recvBuff);
            break;
        }
    }
    close(sockfd);
    return 0;
}
