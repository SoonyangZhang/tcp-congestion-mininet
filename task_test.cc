#include "tcp/network_thread.h"
#include <iostream>
void TaskCallback(void *arg);
class TestCounter{
public:
	TestCounter(tcp::NetworkThread *thread){
		thread_=thread;
		thread_->PostTask(TaskCallback,this);
	}
	void  RunCounter(){
		std::cout<<"c "<<cout_<<std::endl;
		cout_++;
		thread_->PostTask(TaskCallback,this);
	}
private:
	tcp::NetworkThread *thread_;
	int cout_{0};
};
void TaskCallback(void *arg){
	TestCounter *counter=static_cast<TestCounter*>(arg);
	counter->RunCounter();
}
int main(){
	tcp::NetworkThread thread;
	TestCounter counter(&thread);
	thread.TriggerTasksLibEvent();
	thread.Dispatch();
}
