#pragma once
#include "base/lock.h"
#include <pthread.h>
#include <utility>
#include <memory>
#include <event2/event_struct.h>
#include <event2/event.h>
#include <list>
namespace tcp{
typedef void (*TaskCallback)(void*);
class Task{
public:
	Task(TaskCallback fun,void*arg)
	:fun_(fun),arg_(arg){}
	~Task(){}
	void Execute(){
		if(fun_){
			fun_(arg_);
		}
	}
private:
	TaskCallback fun_{nullptr};
	void* arg_;
};
class TaskQueue{
public:
	TaskQueue(){}
	~TaskQueue(){}
	void PostTask(TaskCallback fun,void*arg);
	void RunTasks();
private:
	base::AtomicLock lock_;
	std::list<std::shared_ptr<Task>> tasks_;
};
class NetworkThread{
public:
	NetworkThread();
	~NetworkThread();
	void Dispatch();
	void TriggerTasksLibEvent();
	void PostTask(TaskCallback fun,void*arg);
	struct event_base *getEventBase() {
		return evb_;
	}
	void PollTaskQueue();
private:
	void RegisterToLibEvent();
	struct event_base *evb_;
	struct event event_tasks_;
	TaskQueue taskQueue_;
};
}
