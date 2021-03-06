#pragma once
#include "logging/dmlc_logging.h"
#define QUIC_BUG_IMPL LOG(FATAL)
#define QUIC_BUG_IF_IMPL(condition) LOG_IF(FATAL, condition)
#define QUIC_PEER_BUG_IMPL LOG(ERROR)
#define QUIC_PEER_BUG_IF_IMPL(condition) LOG_IF(ERROR, condition)

#define QUIC_BUG QUIC_BUG_IMPL
#define QUIC_BUG_IF QUIC_BUG_IF_IMPL
#define QUIC_PEER_BUG QUIC_PEER_BUG_IMPL
#define QUIC_PEER_BUG_IF QUIC_PEER_BUG_IF_IMPL
