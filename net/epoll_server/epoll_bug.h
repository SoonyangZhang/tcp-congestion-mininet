#pragma once
#include "epoll_logging.h"
#define EPOLL_BUG_IMPL EPOLL_LOG_IMPL(FATAL)
#define EPOLL_BUG EPOLL_BUG_IMPL