#include "base_thread.h"
#include <unistd.h>
namespace base{
ThreadRef GetCurrentThreadRef(){
    return pthread_self();
}
bool IsThreadRefEqual(const ThreadRef& a, const ThreadRef& b){
    return pthread_equal(a,b);
} 
ThreadManager *ThreadManager::Instance(){
    static ThreadManager  *const thread_manager
    =new ThreadManager();
    return thread_manager;
}
BaseThread* ThreadManager::CurrentThread(){
     return static_cast<BaseThread*>(pthread_getspecific(key_));
}
void ThreadManager::SetCurrentThread(BaseThread *thread){
    pthread_setspecific(key_, thread);    
}
bool ThreadManager::IsMainThread(){
    return IsThreadRefEqual(GetCurrentThreadRef(),main_thread_ref_);
}
ThreadManager::ThreadManager():main_thread_ref_(GetCurrentThreadRef()){
     pthread_key_create(&key_, nullptr);
}
void *thread_fun(void *param){
    BaseThread *thread=static_cast<BaseThread*>(param);
    ThreadManager::Instance()->SetCurrentThread(thread);
    thread->Run();
    return;
}
//seems not safe
BaseThread *BaseThread::Current(){
    ThreadManager *manager=ThreadManager::Instance();
    BaseThread *thread=manager->CurrentThread();
    /*if((!thread)&&manager->IsMainThread()){
        thread=new BaseThread();
    }*/
    return thread;
}
bool BaseThread::IsCurrent() const{
    return this==ThreadManager::Instance()->CurrentThread();
}
bool BaseThread::Start(){
    int ret=0;
    ret=pthread_create(&pid_,nullptr,thread_fun,(void*)this);
    if(ret){
        return false;
    }
    return true;

}
void BaseThread::Stop(){
	running_=false;
    pthread_join(pid_,nullptr);
}
void BaseThread::Sleep(int ms){
    uint64_t us=ms*1000;
    usleep(us);
}
}
