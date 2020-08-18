#pragma once
#include "logging.h"
namespace epoll_server {

#define EPOLL_LOG_IMPL(severity) DLOG(severity)
#define EPOLL_VLOG_IMPL(verbose_level) DLOG(INFO)
#define EPOLL_DVLOG_IMPL(verbose_level) DLOG(INFO)
#define EPOLL_PLOG_IMPL(severity) DLOG(INFO)


#define EPOLL_LOG(severity) EPOLL_LOG_IMPL(severity)
#define EPOLL_VLOG(verbosity) EPOLL_VLOG_IMPL(verbosity)
#define EPOLL_DVLOG(verbosity) EPOLL_DVLOG_IMPL(verbosity)
#define EPOLL_PLOG(severity) EPOLL_PLOG_IMPL(severity)

}
