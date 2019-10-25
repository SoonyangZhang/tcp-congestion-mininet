#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "ae.h"
#include "anet.h"
#include "logging.h"
#include "cmdline.h"
#include "proto_time.h"
#include "byte_codec.h"
#include "tcp_trace.h"
const int kBufferLen=1500;
char g_err_string[kBufferLen];
const int kListenBacklog=128;
aeEventLoop *g_event_loop = NULL;
using namespace std;
int PrintTimer(struct aeEventLoop *eventLoop, long long id, void *clientData)
{
    static int i = 0;
    printf("Test Output: %d\n", i++);
    return 1000;//1 s
}
void ClientClose(aeEventLoop *el, int fd, int err)
{
    //如果err为0，则说明是正常退出，否则就是异常退出
    if( 0 == err )
        printf("Client quit: %d\n", fd);
    else if( -1 == err )
        fprintf(stderr, "Client Error: %s\n", strerror(errno));

    //删除结点，关闭文件
    aeDeleteFileEvent(el, fd, AE_READABLE);
    close(fd);
}
int client_id=0;
int BytesTotal=0;
int BytesRead=0;
char *done_msg="read done";
bool first_packet=true;
const int kLogInterval=5000;
int nextLogTime=0;
int64_t startTime=0;
tcp::TcpTrace *g_trace=nullptr;
int64_t GetWallTime(){
	int64_t now=base::TimeMillis()-startTime;
	return now;
}
void SendReadDone(int fd){
	char buffer[kBufferLen] = {0};
	memset(buffer,0,kBufferLen);
	int msglen=strlen(done_msg);
	memcpy(buffer,done_msg,msglen);
	int res=write(fd,buffer,msglen+1);
	if( -1 == res ){
		LOG(INFO)<<"write error";
	}
}
void LogReadBytes(int64_t now){
	if(g_trace){
		g_trace->OnRecvData(client_id,now,BytesRead);
	}
}
void ReadFromClient(aeEventLoop *el, int fd, void *privdata, int mask)
{
    int64_t now=GetWallTime();
	char buffer[kBufferLen] = { 0 };
    int res;
    int onceRead=0;
    while(true){
        res = read(fd, buffer, kBufferLen);
        if( res <= 0 )
        {
        	if(errno == EWOULDBLOCK || errno == EAGAIN){
        		//LOG(INFO)<<"no data";
        	}else{
            	LOG(INFO)<<"read error";
                ClientClose(el, fd, res);
        	}
            break;
        }
	onceRead+=res;
        if(res>0){
            if(first_packet){
            	basic::DataReader reader(buffer,res);
            	uint32_t first,second;
            	reader.ReadUInt32(&first);
            	reader.ReadUInt32(&second);
        		LOG(INFO)<<"recv "<<first<<" "<<second;
        		client_id=first;
        		BytesTotal=second;
        		LogReadBytes(now);
        		nextLogTime=now+kLogInterval;
        		first_packet=false;
            }
            BytesRead+=res;
            if(now>nextLogTime){
            	LogReadBytes(now);
            	nextLogTime=now+kLogInterval;
            }
            if(BytesRead>=BytesTotal){
            	SendReadDone(fd);
            	ClientClose(el, fd, res);
            }
        }
    }
    LOG(INFO)<<"once read "<<onceRead;
}
void AcceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask)
{
    int cfd, cport;
    char ip_addr[128] = { 0 };
    cfd = anetTcpAccept(g_err_string, fd, ip_addr,sizeof(ip_addr),&cport);
    printf("Connected from %s:%d\n", ip_addr, cport);
    anetNonBlock(g_err_string,cfd);
    if( aeCreateFileEvent(el, cfd, AE_READABLE,
        ReadFromClient, NULL) == AE_ERR )
    {
        fprintf(stderr, "client connect fail: %d\n", fd);
        close(fd);
    }
}
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
    a.add<string>("bind", 'b', "bind addr", false, "0.0.0.0");
    a.add<int>("port", 'p', "port number", false,3333, cmdline::range(1, 65535));
    a.add<string>("log", 'l', "log file", false, "redis_server_log.txt");
    a.parse_check(argc, argv);
    std::string local=a.get<string>("bind");
    int port=a.get<int>("port");
    std::string log=a.get<string>("log");
    g_trace=new tcp::TcpTrace(log);
    char bind_ip[32]={0};
    memcpy(bind_ip,local.c_str(),local.size());
    printf("Start\n");
    startTime=base::TimeMillis();
    g_event_loop = aeCreateEventLoop(1024*10);
    int fd = anetTcpServer(g_err_string,port, bind_ip,kListenBacklog);
    if( ANET_ERR == fd )
        fprintf(stderr, "Open port %d error: %s\n", port, g_err_string);
    anetNonBlock(g_err_string,fd);
    if( aeCreateFileEvent(g_event_loop, fd, AE_READABLE, 
        AcceptTcpHandler, NULL) == AE_ERR )
        fprintf(stderr, "Unrecoverable error creating server.ipfd file event.");
    //aeCreateTimeEvent(g_event_loop, 1, PrintTimer, NULL, NULL);
    aeSetDontWait(g_event_loop,1);
    while(m_running){
    	aeLoopOnce(g_event_loop);
    }
    aeDeleteEventLoop(g_event_loop);
    close(fd);
    delete g_trace;
    g_trace=nullptr;
    printf("End\n");
    return 0;
}
