PROJECT(project)
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS "-fPIC")  
add_definitions(-D__STDC_FORMAT_MACROS)
add_definitions(-DNDEBUG)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_definitions(-D__linux__)
add_definitions(-std=c++11)
include_directories(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib)
include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/logging)
include_directories(${CMAKE_SOURCE_DIR}/tcp)
include_directories(${CMAKE_SOURCE_DIR}/net)
include_directories(${CMAKE_SOURCE_DIR}/net/basic)
include_directories(${CMAKE_SOURCE_DIR}/net/epoll_server)
include_directories(${CMAKE_SOURCE_DIR}/net/quic)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/redisnet/)
set(base_LIB
base/byte_codec.cc
base/base_thread.cc
base/callback.cc
base/proto_time.cc
base/random.cc
)
set(net_LIB
net/basic/quic_bug_tracker.h        
net/basic/quic_ip_address.h       
net/basic/quic_socket_address.h
net/basic/quic_export.h 
net/basic/quic_string_utils.h
net/basic/quic_net_utils.h
net/basic/quic_ip_address_family.h            
net/basic/quic_net_utils.cc       
net/basic/quic_ip_address.cc        
net/basic/quic_socket_address.cc
net/epoll_server/epoll_bug.h     
net/epoll_server/epoll_logging.h 
net/epoll_server/epoll_export.h  
net/epoll_server/epoll_time.h     
net/epoll_server/simple_epoll_server.h
net/epoll_server/simple_epoll_server.cc
net/quic/quic_alarm.cc
net/quic/quic_bandwidth.cc
net/quic/quic_clock.cc
net/quic/quic_epoll_alarm_factory.cc
net/quic/quic_epoll_clock.cc
net/quic/quic_time.cc

)
add_library(base STATIC ${base_LIB})
add_library(net STATIC ${net_LIB})
set(tcp_LIB
tcp/network_thread.cc
tcp/tcp_client.cc
tcp/tcp_peer.cc
tcp/tcp_server.cc
tcp/tcp_trace.cc
)
add_library(tcp STATIC ${tcp_LIB})

set(redisnet_LIB
thirdparty/redisnet/ae.c
thirdparty/redisnet/anet.c
thirdparty/redisnet/zmalloc.c
)
add_library(redisnet STATIC ${redisnet_LIB})



set(EXECUTABLE_NAME "t_cmd")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/thirdparty/test2.cpp)

set(EXECUTABLE_NAME "echo_client")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/echo_client.cc)
target_link_libraries(${EXECUTABLE_NAME} tcp redisnet base  pthread)

set(EXECUTABLE_NAME "block_client")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/block_client.cc)
target_link_libraries(${EXECUTABLE_NAME} base)

set(EXECUTABLE_NAME "echo_server")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/echo_server.cc)
target_link_libraries(${EXECUTABLE_NAME} tcp redisnet base   pthread)


set(EXECUTABLE_NAME "t_task")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/task_test.cc)
target_link_libraries(${EXECUTABLE_NAME} tcp redisnet base  pthread)

set(EXECUTABLE_NAME "tcp_info")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/tcp_info.cc)


set(EXECUTABLE_NAME "t_redis")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/redis-main.cc)
target_link_libraries(${EXECUTABLE_NAME}  tcp redisnet base)


set(EXECUTABLE_NAME "t_net")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/net/net-test.cc)
target_link_libraries(${EXECUTABLE_NAME}  net base)


