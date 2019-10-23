#pragma once
#include <stdint.h>
#include <pthread.h>
namespace base{
typedef pthread_t ThreadRef;
class  BaseThread;
ThreadRef GetCurrentThreadRef();
bool IsThreadRefEqual(const ThreadRef& a, const ThreadRef& b);
class ThreadManager{
public:
    static ThreadManager* Instance();
    BaseThread* CurrentThread();
    void SetCurrentThread(BaseThread *thread);
    bool IsMainThread();
private:
    ThreadManager();
    ~ThreadManager(){}
    pthread_key_t key_;
    const ThreadRef main_thread_ref_;
};
typedef void (*ThreadRunFunction)(void*);
class BaseThread{
public:
    static BaseThread *Current();
    BaseThread(){}
    BaseThread(ThreadRunFunction fun,void*obj):fun_(fun),obj_(obj){}
    bool IsCurrent() const;
    bool Start();
    void Stop();
    static void Sleep(int ms);
    virtual void Run(){
        if(fun_&&running_){
            fun_(obj_);
        }
    }
private:
    ThreadRunFunction fun_{nullptr};
    void *obj_{nullptr};
    ThreadRef pid_;
    bool running_{true};
};
}