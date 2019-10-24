PROJECT(project)
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS "-fPIC")  
add_definitions(-D__STDC_FORMAT_MACROS)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)


include_directories(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib)
include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/tcp)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty)
set(base_LIB
base/byte_codec.cc
base/base_thread.cc
base/callback.cc
base/proto_time.cc
base/random.cc
)
add_library(base STATIC ${base_LIB})

set(tcp_LIB
tcp/network_thread.cc
tcp/tcp_client.cc
tcp/tcp_peer.cc
tcp/tcp_server.cc
tcp/tcp_trace.cc
)
add_library(tcp STATIC ${tcp_LIB})

set(EXECUTABLE_NAME "t_event_server")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/event_server.cc)
target_link_libraries(${EXECUTABLE_NAME} event)

set(EXECUTABLE_NAME "t_event_client")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/event_client.cc)
target_link_libraries(${EXECUTABLE_NAME} event)

set(EXECUTABLE_NAME "tcp_info")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/tcp_info.cc)
target_link_libraries(${EXECUTABLE_NAME} base event)

set(EXECUTABLE_NAME "t_cmd")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/thirdparty/test2.cpp)

set(EXECUTABLE_NAME "echo_client")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/echo_client.cc)
target_link_libraries(${EXECUTABLE_NAME} tcp base event  pthread)


set(EXECUTABLE_NAME "echo_server")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/echo_server.cc)
target_link_libraries(${EXECUTABLE_NAME} tcp base event  pthread)


set(EXECUTABLE_NAME "t_task")
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/task_test.cc)
target_link_libraries(${EXECUTABLE_NAME} tcp base event  pthread)