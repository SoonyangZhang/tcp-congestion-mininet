#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/tcp.h>
#include <iostream>
#include "dmlc_logging.h"
static const size_t TCP_CC_NAME_MAX = 16;
void error_handling(char *message);
void print_cc_type(int fd){
    char optval[TCP_CC_NAME_MAX];
    memset(optval,0,TCP_CC_NAME_MAX);
    int length=sizeof(optval);
    getsockopt(fd,IPPROTO_TCP, TCP_CONGESTION, (void*)optval,(socklen_t*)&length);
    printf("cctype %s\n",optval);
}
void print_tcp_send_buf(int fd){
    int buf_len;
    socklen_t len=sizeof(buf_len);
    getsockopt(fd,SOL_SOCKET,SO_SNDBUF,(void *)&buf_len,&len);
    LOG(INFO)<<"send buf "<<buf_len;
}
void print_tcp_recv_buf(int fd){
    int buf_len;
    socklen_t len=sizeof(buf_len);
    getsockopt(fd,SOL_SOCKET,SO_RCVBUF,(void *)&buf_len,&len);
    LOG(INFO)<<"recv buf "<<buf_len;    
}
int set_congestion_type(int fd,char *cc){
    char optval[TCP_CC_NAME_MAX];
    memset(optval,0,TCP_CC_NAME_MAX);
    strncpy(optval,cc,TCP_CC_NAME_MAX);
    int length=strlen(optval)+1;
    int rc=setsockopt(fd,IPPROTO_TCP, TCP_CONGESTION, (void*)optval,length);
    if(rc!=0){
        printf("cc is not supprt\n");
    }
    return rc;
}
using namespace std;
int main(int argc, char* argv[])
{
    int sock;
    char *cc_type="cubic";
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    set_congestion_type(sock,cc_type);
    print_cc_type(sock);
    print_tcp_send_buf(sock);
    print_tcp_recv_buf(sock);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
