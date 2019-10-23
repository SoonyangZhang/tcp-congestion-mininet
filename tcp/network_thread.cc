#include <event2/thread.h>
#include <time.h>
#include "network_thread.h"
#include "logging.h"
namespace tcp{
void TaskQueue::PostTask(TaskCallback fun,void*arg){
	base::LockScope ls(&lock_);
	std::shared_ptr<Task> task=std::make_shared<Task>(fun,arg);
	tasks_.push_back(task);
}
void TaskQueue::RunTasks(){
	std::list<std::shared_ptr<Task>> tasks;
	{
		base::LockScope ls(&lock_);
		tasks.swap(tasks_);
	}
	while(!tasks.empty()){

		auto it=tasks.begin();
		(*it)->Execute();
		tasks.erase(it);
	}
}
void RunTaskCallback(evutil_socket_t fd, short event, void *arg){
	NetworkThread *thread=static_cast<NetworkThread*>(arg);
	thread->PollTaskQueue();
}
NetworkThread::NetworkThread(){
	evthread_use_pthreads();
	evb_=::event_base_new();
}
NetworkThread::~NetworkThread(){

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
void NetworkThread::PostTask(TaskCallback fun,void*arg){
	taskQueue_.PostTask(fun,arg);
}
void NetworkThread::PollTaskQueue(){
	taskQueue_.RunTasks();
	RegisterToLibEvent();
}
}

