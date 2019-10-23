#include "tcp/network_thread.h"
#include <iostream>
class TestCounter{
public:
	TestCounter(tcp::NetworkThread *thread){
		thread_=thread;
		PostTaskAgain();
	}
	void  RunCounter(){
		std::cout<<"c "<<cout_<<std::endl;
		cout_++;
		PostTaskAgain();
	}
	void  PostTaskAgain(){
		thread_->PostTask([this](){
			RunCounter();
		});
	}
private:
	tcp::NetworkThread *thread_;
	int cout_{0};
};
int main(){
	tcp::NetworkThread thread;
	TestCounter counter(&thread);
	thread.TriggerTasksLibEvent();
	thread.Dispatch();
}
