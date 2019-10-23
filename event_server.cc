//from https://blog.csdn.net/ljp1919/article/details/48163091
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
 
#define LISTEN_PORT 8888
#define LISTEN_BACKLOG 32
 
void do_accept(evutil_socket_t listener, short event, void *arg);
void read_cb(struct bufferevent *bev, void *arg);
void error_cb(struct bufferevent *bev, short event, void *arg);
void write_cb(struct bufferevent *bev, void *arg);
bool m_running=true;
void signal_exit_handler(int sig)
{
	m_running=false;
}
int main()
{
	signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGTSTP, signal_exit_handler);
    //int ret;
    evutil_socket_t listener;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    assert(listener > 0);
    evutil_make_listen_socket_reuseable(listener);
 
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(LISTEN_PORT);
 
    if (bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return 1;
    }
 
    if (listen(listener, LISTEN_BACKLOG) < 0) {
        perror("listen");
        return 1;
    }
 
    printf ("Listening...\n");
 
    evutil_make_socket_nonblocking(listener);
 
    struct event_base *base = event_base_new();
    assert(base != NULL);
    struct event *listen_event;
    listen_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);
    event_add(listen_event, NULL);
    while(m_running){
    	event_base_loop(base, EVLOOP_ONCE | EVLOOP_NONBLOCK);
    }
    //event_base_dispatch(base);
    event_base_free(base);
    evutil_closesocket(listener);
    printf("The End.");
    return 0;
}
 
void do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = (struct event_base *)arg;
    evutil_socket_t fd;
    struct sockaddr_in sin;
    socklen_t slen = sizeof(sin);
    fd = accept(listener, (struct sockaddr *)&sin, &slen);
    if (fd < 0) {
        perror("accept");
        return;
    }
    if (fd > FD_SETSIZE) {
        perror("fd > FD_SETSIZE\n");
        return;
    }
 
    printf("ACCEPT: fd = %u\n", fd);
 
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, read_cb, NULL, error_cb, arg);
    bufferevent_enable(bev, EV_READ|EV_WRITE|EV_PERSIST);
}
 
void read_cb(struct bufferevent *bev, void *arg)
{
#define MAX_LINE    256
    char line[MAX_LINE+1];
    int n;
    evutil_socket_t fd = bufferevent_getfd(bev);
 
    while (n = bufferevent_read(bev, line, MAX_LINE), n > 0) {
        line[n] = '\0';
        printf("fd=%u, read line: %s\n", fd, line);
        bufferevent_write(bev, line, n);
    }
}
 
void write_cb(struct bufferevent *bev, void *arg) {}
 
void error_cb(struct bufferevent *bev, short event, void *arg)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    printf("fd = %u, ", fd);
    if (event & BEV_EVENT_TIMEOUT) {
        printf("Timed out\n"); //if bufferevent_set_timeouts() called
    }
    else if (event & BEV_EVENT_EOF) {
        printf("connection closed\n");
    }
    else if (event & BEV_EVENT_ERROR) {
        printf("some other error\n");
    }
    bufferevent_free(bev);
}
