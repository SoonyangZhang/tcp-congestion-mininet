#pragma once
#include "base/lock.h"
#include <pthread.h>
#include <event2/event_struct.h>
#include <event2/event.h>
#include "task_queue.h"
#include "base/min_heap.h"
namespace tcp{
class NetworkThread{
public:
	NetworkThread();
	~NetworkThread();
	void Dispatch();
	void TriggerTasksLibEvent();
	struct event_base *getEventBase() {
		return evb_;
	}
	  template <class Closure,
	            typename std::enable_if<!std::is_convertible<
	                Closure,
	                std::unique_ptr<QueuedTask>>::value>::type* = nullptr>
	    void PostTask(Closure&& closure) {
	    PostTask(NewClosure(std::forward<Closure>(closure)));
	  }
	  template <class Closure,
	            typename std::enable_if<!std::is_convertible<
	                Closure,
	                std::unique_ptr<QueuedTask>>::value>::type* = nullptr>
	  void PostDelayedTask(Closure&& closure, uint32_t time_ms) {
	    PostDelayedTask(NewClosure(std::forward<Closure>(closure)), time_ms);
	  }
	void PostDelayedTask(std::unique_ptr<QueuedTask> task, uint32_t time_ms);
	void PostTask(std::unique_ptr<QueuedTask>task);
	void PollTaskQueue();
private:
	void ProcessTasks();
	void RegisterToLibEvent();
	void Clear();
	struct event_base *evb_;
	struct event event_tasks_;
	base::AtomicLock pending_lock_;
	base::min_heap<TaskEvent> s_heap_;
};
}
