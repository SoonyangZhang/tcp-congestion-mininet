#include <event2/thread.h>
#include <time.h>
#include <list>
#include "proto_time.h"
#include "network_thread.h"
#include "logging.h"
using namespace base;
namespace tcp{
void RunTaskCallback(evutil_socket_t fd, short event, void *arg){
	NetworkThread *thread=static_cast<NetworkThread*>(arg);
	thread->PollTaskQueue();
}
NetworkThread::NetworkThread(){
	evthread_use_pthreads();
	evb_=::event_base_new();
	min_heap_ctor(&s_heap_);
}
NetworkThread::~NetworkThread(){
	Clear();
	min_heap_dtor(&s_heap_);
}
void NetworkThread::RegisterToLibEvent(){
	struct timeval tv;
	event_assign(&event_tasks_, evb_, -1, 0, RunTaskCallback, (void*)this);
	evutil_timerclear(&tv);
	tv.tv_sec = 0;
	event_add(&event_tasks_, &tv);
}
void NetworkThread::TriggerTasksLibEvent(){
	RegisterToLibEvent();
}
void NetworkThread::Dispatch(){
	::event_base_dispatch(evb_);
}
void NetworkThread::Loop(){
	event_base_loop(evb_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
}
void NetworkThread::PostTask(std::unique_ptr<QueuedTask>task)
{
	PostDelayedTask(std::move(task),0);
}
void NetworkThread::PostDelayedTask(std::unique_ptr<QueuedTask> task, uint32_t time_ms)
{

    uint64_t now=base::TimeMillis();
    uint64_t future=now+time_ms;
    TaskEvent *event=new TaskEvent(future,std::move(task));
    base::LockScope lock(&pending_lock_);
    min_heap_push(&s_heap_,event);
}
void NetworkThread::PollTaskQueue(){
	ProcessTasks();
	RegisterToLibEvent();
}
void NetworkThread::ProcessTasks(){
    uint64_t time_ms=base::TimeMillis();
    std::list<std::unique_ptr<QueuedTask>> pending_tasks;
    {

        base::LockScope lock(&pending_lock_);
        while(!min_heap_empty(&s_heap_)){
        TaskEvent *event=min_heap_top(&s_heap_);
        if(event->delay<=time_ms){
            pending_tasks.push_back(std::move(event->task));
            delete event;
            min_heap_pop(&s_heap_);
        }
        else{
            break;
            }
        }
    }
    while(!pending_tasks.empty())
    {
        std::unique_ptr<QueuedTask> task=std::move(pending_tasks.front());//move ownership
        pending_tasks.pop_front();
        task->Run();
    }
}
void NetworkThread::Clear(){
	while(!min_heap_empty(&s_heap_)){
	TaskEvent *e=min_heap_top(&s_heap_);
	min_heap_pop(&s_heap_);
	delete e;
	}
}
}

