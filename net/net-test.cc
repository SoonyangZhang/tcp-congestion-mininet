#include <iostream>
#include <utility>
#include <signal.h>
#include "quic_ip_address.h"
#include "quic_socket_address.h"
#include "quic_time.h"
#include "quic_clock.h"
#include "quic_epoll_alarm_factory.h"
#include "quic_epoll.h"
#include "quic_epoll_clock.h"
namespace quic{
class TestAlarmObjectInterface{
public: 
    virtual void  Count()=0;
    virtual ~TestAlarmObjectInterface(){}
};
// An alarm that is scheduled to send an ack if a timeout occurs.
class TestAlarmDelegate : public QuicAlarm::Delegate {
 public:
  explicit TestAlarmDelegate(TestAlarmObjectInterface* obj)
      : object_(obj) {}
  TestAlarmDelegate(const TestAlarmDelegate&) = delete;
  TestAlarmDelegate& operator=(const TestAlarmDelegate&) = delete;

  void OnAlarm() override {
        object_->Count();
  }
 private:
  TestAlarmObjectInterface* object_;
};
class TestAlarmObject:public TestAlarmObjectInterface{
public:
    TestAlarmObject(QuicAlarmFactory* alarm_factory,QuicClock *clock):
    alarm_factory_(alarm_factory),
    clock_(clock){
      alarm_=alarm_factory_->CreateAlarm(arena_.New<TestAlarmDelegate>(this),
                                             &arena_);
      alarm_->Update(clock_->ApproximateNow(),QuicTime::Delta::Zero());                                       
    }
    ~TestAlarmObject() override{}
    void Count() override{
        count_++;
        std::cout<<"counter "<<count_<<std::endl;
        if(count_<10){
            UpdateAlarm();
        }
    }
    void UpdateAlarm(){
        if(alarm_->IsSet()){
            alarm_->Cancel();
        }
        QuicTime deadline=clock_->ApproximateNow()+QuicTime::Delta::FromMilliseconds(500);
        alarm_->Update(deadline,QuicTime::Delta::Zero());
    }
private:
    QuicAlarmFactory *alarm_factory_=nullptr;
    QuicClock *clock_=nullptr;
    QuicArenaScopedPtr<QuicAlarm> alarm_;
    QuicOneBlockArena<1024> arena_;
    int count_=0;
};
}
bool m_running=true;
void signal_exit_handler(int sig)
{
    m_running=false;
}
using namespace quic;
int main()
{
QuicIpAddress ip;
ip.FromString("139.169.255.0");
QuicSocketAddress addr(ip,1234);
std::cout<<addr<<std::endl;
QuicEpollServer eps;
QuicEpollClock clock(&eps);
std::unique_ptr<QuicEpollAlarmFactory> alarm_factory(new QuicEpollAlarmFactory(&eps));
TestAlarmObject test(alarm_factory.get(),&clock);
while(m_running){
    eps.WaitForEventsAndExecuteCallbacks();
}
}
