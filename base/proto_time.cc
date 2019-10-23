#if defined (WIN_32)
#include <windows.h>
#include <time.h>
#else
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#endif
//__STDC_FORMAT_MACROS PRId64
#include <inttypes.h>
#include "proto_time.h"
static inline void itimeofday(long *sec, long *usec){
	#if defined (WIN_32)
	static long mode = 0, addsec = 0;
	bool retval;
	static int64_t freq = 1;
	int64_t qpc;
	if (mode == 0) {
		retval = QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		freq = (freq == 0)? 1 : freq;
		retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
		addsec = (long)time(NULL);
		addsec = addsec - (long)((qpc / freq) & 0x7fffffff);
		mode = 1;
	}
	retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
	retval = retval * 2;
	if (sec) *sec = (long)(qpc / freq) + addsec;
	if (usec) *usec = (long)((qpc % freq) * 1000000 / freq);
	#else
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;
	#endif
}
static int64_t base_clock64(void){
	long s, u;
	int64_t value;
	itimeofday(&s, &u);
	value = ((int64_t)s) * 1000 + (u / 1000);
	return value;
}
static int32_t base_clock32(void){
	return (int32_t)(base_clock64()& 0xfffffffful);
}

namespace base{
ProtoClock *gclock=nullptr;
void SetClockForTesting(ProtoClock *clock){
    gclock=clock;
}
void TimeSleep(int64_t milliseconds){
	#if defined (WIN_32)
	Sleep(milliseconds);
	#else
	struct timespec req = {0};
	time_t sec = (int)(milliseconds / 1000);
	milliseconds = milliseconds - (sec * 1000);
	req.tv_sec = sec;
	req.tv_nsec = milliseconds * 1000000L;
	nanosleep(&req,NULL);
	#endif
}
int64_t TimeMillis(){
    if(gclock){
        ProtoTime now=gclock->Now();
        TimeDelta delta=now-ProtoTime::Zero();
        return delta.ToMilliseconds();
    }else{
        return base_clock64();
    }
}
int64_t TimeMicro(){
    if(gclock){
        ProtoTime now=gclock->Now();
        TimeDelta delta=now-ProtoTime::Zero();
        return delta.ToMicroseconds();
    }else{
        return base_clock64()*1000;
    }
}
ProtoTime SystemClock::Now() const{
    ProtoTime base=ProtoTime::Zero();
    TimeDelta elapse=TimeDelta::FromMilliseconds(base_clock64());
    ProtoTime now=base+elapse;
    return now;
}
ProtoTime SystemClock::ApproximateNow() const{
    return Now();
}
}
