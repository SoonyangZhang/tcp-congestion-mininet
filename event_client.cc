//客户端代码
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
 
#include <event2/event.h>
#include <event2/bufferevent.h>
 
#define SERV_PORT 9999
#define MAX_LINE 1024
 
void cmd_msg_cb(int fd, short event, void *arg);
void read_cb(struct bufferevent *bev, void *arg);
void error_cb(struct bufferevent *bev, short event, void *arg);
const char *serv_addr="127.0.0.1";
int main(int argc, char *argv[])
{
    evutil_socket_t sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket\n");
        return 1;
    }
 
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if(inet_pton(AF_INET, serv_addr, &servaddr.sin_addr) < 1)
    {
        perror("inet_ntop\n");
        return 1;
    }
    if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect\n");
        return 1;
    }
    evutil_make_socket_nonblocking(sockfd);
 
    printf("Connect to server sucessfully!\n");
    // build event base
    struct event_base *base = event_base_new();
    if(base == NULL)
    {
        perror("event_base\n");
        return 1;
    }
    const char *eventMechanism = event_base_get_method(base);
    printf("Event mechanism used is %s\n", eventMechanism);
    printf("sockfd = %d\n", sockfd);
 
    struct bufferevent *bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
    
    struct event *ev_cmd;
    ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, cmd_msg_cb, (void *)bev);
    event_add(ev_cmd, NULL);
    
    bufferevent_setcb(bev, read_cb, NULL, error_cb, (void *)ev_cmd);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
    
    event_base_dispatch(base);
 
    printf("The End.");
    return 0;
}
 
void cmd_msg_cb(int fd, short event, void *arg)
{
    char msg[MAX_LINE];
    int nread = read(fd, msg, sizeof(msg));
    if(nread < 0)
    {
        perror("stdio read fail\n");
        return;
    }
 
    struct bufferevent *bev = (struct bufferevent *)arg;
    bufferevent_write(bev, msg, nread);
}
 
void read_cb(struct bufferevent *bev, void *arg)
{
    char line[MAX_LINE + 1];
    int n;
    evutil_socket_t fd = bufferevent_getfd(bev);
 
    while((n = bufferevent_read(bev, line, MAX_LINE)) > 0)
    {
        line[n] = '\0';
        printf("fd = %u, read from server: %s", fd, line);
    }
}
 
void error_cb(struct bufferevent *bev, short event, void *arg)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    printf("fd = %u, ", fd);
    if(event & BEV_EVENT_TIMEOUT)
        printf("Time out.\n");  // if bufferevent_set_timeouts() is called
    else if(event & BEV_EVENT_EOF)
        printf("Connection closed.\n");
    else if(event & BEV_EVENT_ERROR)
        printf("Some other error.\n");
    bufferevent_free(bev);
 
    struct event *ev = (struct event *)arg;
    event_free(ev);
}
