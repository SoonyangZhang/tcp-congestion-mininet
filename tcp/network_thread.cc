#include <time.h>
#include <list>
#include "proto_time.h"
#include "network_thread.h"
#include "dmlc_logging.h"
using namespace base;
namespace tcp{
int RunTaskCallback(struct aeEventLoop *eventLoop, long long id, void *arg){
	NetworkThread *thread=static_cast<NetworkThread*>(arg);
	thread->PollTaskQueue();
	return 1;//ms
}
NetworkThread::NetworkThread(){
	evb_=aeCreateEventLoop(1024*10);
	aeSetDontWait(evb_,1);
	aeCreateTimeEvent(evb_, 1, RunTaskCallback, (void*)this, NULL);
	min_heap_ctor(&s_heap_);
}
NetworkThread::~NetworkThread(){
	if(evb_){
		aeDeleteEventLoop(evb_);
		evb_=nullptr;
	}
	Clear();
	min_heap_dtor(&s_heap_);
}
void NetworkThread::LoopOnce(){
	if(evb_){
		aeLoopOnce(evb_);
	}
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

