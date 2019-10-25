#include "tcp/network_thread.h"
#include <iostream>
#include <signal.h>
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
bool m_running=true;
void signal_exit_handler(int sig)
{
	m_running=false;
}
int main(){
    signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGTSTP, signal_exit_handler);
	tcp::NetworkThread thread;
	TestCounter counter(&thread);
    while(m_running){
        thread.LoopOnce();
    }
	
}
