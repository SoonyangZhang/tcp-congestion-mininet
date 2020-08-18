#pragma once
#include "base/proto_time.h"
//#include "butil/time/time.h"
namespace epoll_server {
inline int64_t WallTimeNowInUsecImpl() {
   return base::TimeMicro();
  //return (butil::Time::Now() - butil::Time::UnixEpoch()).InMicroseconds();
}
inline int64_t WallTimeNowInUsec() { return WallTimeNowInUsecImpl();}
}  // namespace epoll_server
