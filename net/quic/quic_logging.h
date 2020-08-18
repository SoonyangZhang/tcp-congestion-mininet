#pragma once
#include "dmlc_logging.h"
#define QUICHE_LOG_IMPL(severity) QUICHE_CHROMIUM_LOG_##severity
#define QUICHE_VLOG_IMPL(verbose_level) VLOG(verbose_level)
#define QUICHE_LOG_EVERY_N_SEC_IMPL(severity, seconds) QUICHE_LOG_IMPL(severity)
#define QUICHE_LOG_FIRST_N_IMPL(severity, n) QUICHE_LOG_IMPL(severity)
#define QUICHE_DLOG_IMPL(severity) QUICHE_CHROMIUM_DLOG_##severity
#define QUICHE_DLOG_IF_IMPL(severity, condition) \
  QUICHE_CHROMIUM_DLOG_IF_##severity(condition)
#define QUICHE_LOG_IF_IMPL(severity, condition) \
  QUICHE_CHROMIUM_LOG_IF_##severity(condition)

#define QUICHE_CHROMIUM_LOG_INFO VLOG(1)
#define QUICHE_CHROMIUM_LOG_WARNING DLOG(WARNING)
#define QUICHE_CHROMIUM_LOG_ERROR DLOG(ERROR)
#define QUICHE_CHROMIUM_LOG_FATAL LOG(FATAL)
#define QUICHE_CHROMIUM_LOG_DFATAL LOG(FATAL)

#define QUICHE_CHROMIUM_DLOG_INFO VLOG(1)
#define QUICHE_CHROMIUM_DLOG_WARNING DLOG(WARNING)
#define QUICHE_CHROMIUM_DLOG_ERROR DLOG(ERROR)
#define QUICHE_CHROMIUM_DLOG_FATAL DLOG(FATAL)
#define QUICHE_CHROMIUM_DLOG_DFATAL DLOG(FATAL)

#define QUICHE_CHROMIUM_LOG_IF_INFO(condition) DLOG_IF(INFO, condition)
#define QUICHE_CHROMIUM_LOG_IF_WARNING(condition) DLOG_IF(WARNING, condition)
#define QUICHE_CHROMIUM_LOG_IF_ERROR(condition) DLOG_IF(ERROR, condition)
#define QUICHE_CHROMIUM_LOG_IF_FATAL(condition) LOG_IF(FATAL, condition)
#define QUICHE_CHROMIUM_LOG_IF_DFATAL(condition) LOG_IF(DFATAL, condition)

#define QUICHE_CHROMIUM_DLOG_IF_INFO(condition) DLOG_IF(INFO, condition)
#define QUICHE_CHROMIUM_DLOG_IF_WARNING(condition) DLOG_IF(WARNING, condition)
#define QUICHE_CHROMIUM_DLOG_IF_ERROR(condition) DLOG_IF(ERROR, condition)
#define QUICHE_CHROMIUM_DLOG_IF_FATAL(condition) DLOG_IF(FATAL, condition)
#define QUICHE_CHROMIUM_DLOG_IF_DFATAL(condition) DLOG_IF(FATAL, condition)

#define QUICHE_DVLOG_IMPL(verbose_level) VLOG(verbose_level)
#define QUICHE_DVLOG_IF_IMPL(verbose_level, condition) \
  LOG_IF(INFO, condition)

#define QUICHE_LOG_INFO_IS_ON_IMPL() 0
#ifdef NDEBUG
#define QUICHE_LOG_WARNING_IS_ON_IMPL() 0
#define QUICHE_LOG_ERROR_IS_ON_IMPL() 0
#else
#define QUICHE_LOG_WARNING_IS_ON_IMPL() 1
#define QUICHE_LOG_ERROR_IS_ON_IMPL() 1
#endif
#define QUICHE_DLOG_INFO_IS_ON_IMPL() 0

#if defined(OS_WIN)
// wingdi.h defines ERROR to be 0. When we call QUICHE_DLOG(ERROR), it gets
// substituted with 0, and it expands to QUICHE_CHROMIUM_DLOG_0. To allow us to
// keep using this syntax, we define this macro to do the same thing as
// QUICHE_CHROMIUM_DLOG_ERROR.
#define QUICHE_CHROMIUM_LOG_0 QUICHE_CHROMIUM_LOG_ERROR
#define QUICHE_CHROMIUM_DLOG_0 QUICHE_CHROMIUM_DLOG_ERROR
#define QUICHE_CHROMIUM_LOG_IF_0 QUICHE_CHROMIUM_LOG_IF_ERROR
#define QUICHE_CHROMIUM_DLOG_IF_0 QUICHE_CHROMIUM_DLOG_IF_ERROR
#endif

#define QUICHE_PREDICT_FALSE_IMPL(x) x
#define QUICHE_PREDICT_TRUE_IMPL(x) x

#define QUICHE_NOTREACHED_IMPL() NOTREACHED()

#define QUICHE_PLOG_IMPL(severity) DVLOG(1)



#define QUICHE_DVLOG(verbose_level) QUICHE_DVLOG_IMPL(verbose_level)
#define QUICHE_DVLOG_IF(verbose_level, condition) \
  QUICHE_DVLOG_IF_IMPL(verbose_level, condition)
#define QUICHE_DLOG(severity) QUICHE_DLOG_IMPL(severity)
#define QUICHE_DLOG_IF(severity, condition) \
  QUICHE_DLOG_IF_IMPL(severity, condition)
#define QUICHE_VLOG(verbose_level) QUICHE_VLOG_IMPL(verbose_level)
#define QUICHE_LOG(severity) QUICHE_LOG_IMPL(severity)
#define QUICHE_LOG_FIRST_N(severity, n) QUICHE_LOG_FIRST_N_IMPL(severity, n)
#define QUICHE_LOG_EVERY_N_SEC(severity, seconds) \
  QUICHE_LOG_EVERY_N_SEC_IMPL(severity, seconds)
#define QUICHE_LOG_IF(severity, condition) \
  QUICHE_LOG_IF_IMPL(severity, condition)

#define QUICHE_PREDICT_FALSE(x) QUICHE_PREDICT_FALSE_IMPL(x)
#define QUICHE_PREDICT_TRUE(x) QUICHE_PREDICT_TRUE_IMPL(x)

// This is a noop in release build.
#define QUICHE_NOTREACHED() QUICHE_NOTREACHED_IMPL()

#define QUICHE_PLOG(severity) QUICHE_PLOG_IMPL(severity)

#define QUICHE_DLOG_INFO_IS_ON() QUICHE_DLOG_INFO_IS_ON_IMPL()
#define QUICHE_LOG_INFO_IS_ON() QUICHE_LOG_INFO_IS_ON_IMPL()
#define QUICHE_LOG_WARNING_IS_ON() QUICHE_LOG_WARNING_IS_ON_IMPL()
#define QUICHE_LOG_ERROR_IS_ON() QUICHE_LOG_ERROR_IS_ON_IMPL()

// Please note following QUIC_LOG are platform dependent:
// INFO severity can be degraded (to VLOG(1) or DVLOG(1)).
// Some platforms may not support QUIC_LOG_FIRST_N or QUIC_LOG_EVERY_N_SEC, and
// they would simply be translated to LOG.

#define QUIC_DVLOG QUICHE_DVLOG
#define QUIC_DVLOG_IF QUICHE_DVLOG_IF
#define QUIC_DLOG QUICHE_DLOG
#define QUIC_DLOG_IF QUICHE_DLOG_IF
#define QUIC_VLOG QUICHE_VLOG
#define QUIC_LOG QUICHE_LOG
#define QUIC_LOG_FIRST_N QUICHE_LOG_FIRST_N
#define QUIC_LOG_EVERY_N_SEC QUICHE_LOG_EVERY_N_SEC
#define QUIC_LOG_IF QUICHE_LOG_IF

#define QUIC_PREDICT_FALSE QUICHE_PREDICT_FALSE
#define QUIC_PREDICT_TRUE QUICHE_PREDICT_TRUE

// This is a noop in release build.
#define QUIC_NOTREACHED QUICHE_NOTREACHED

#define QUIC_PLOG QUICHE_PLOG

#define QUIC_DLOG_INFO_IS_ON QUICHE_DLOG_INFO_IS_ON
#define QUIC_LOG_INFO_IS_ON QUICHE_LOG_INFO_IS_ON
#define QUIC_LOG_WARNING_IS_ON QUICHE_LOG_WARNING_IS_ON
#define QUIC_LOG_ERROR_IS_ON QUICHE_LOG_ERROR_IS_ON
