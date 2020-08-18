#pragma once
#include "base/proto_time.h"
namespace epoll_server {
inline int64_t WallTimeNowInUsec() { return base::TimeMicro(); }
}  // namespace epoll_server