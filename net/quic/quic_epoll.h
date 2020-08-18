// Copyright (c) 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef QUICHE_QUIC_PLATFORM_API_QUIC_EPOLL_H_
#define QUICHE_QUIC_PLATFORM_API_QUIC_EPOLL_H_

#include "simple_epoll_server.h"

namespace quic {
using QuicEpollServerImpl = epoll_server::SimpleEpollServer;
using QuicEpollEventImpl = epoll_server::EpollEvent;
using QuicEpollAlarmBaseImpl = epoll_server::EpollAlarm;
using QuicEpollCallbackInterfaceImpl = epoll_server::EpollCallbackInterface;

using QuicEpollServer = QuicEpollServerImpl;
using QuicEpollEvent = QuicEpollEventImpl;
using QuicEpollAlarmBase = QuicEpollAlarmBaseImpl;
using QuicEpollCallbackInterface = QuicEpollCallbackInterfaceImpl;

}  // namespace quic

#endif  // QUICHE_QUIC_PLATFORM_API_QUIC_EPOLL_H_