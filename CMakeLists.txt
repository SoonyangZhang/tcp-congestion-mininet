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
set(tcp_LIB
tcp/network_thread.cc
tcp/tcp_client.cc
tcp/tcp_peer.cc
tcp/tcp_server.cc
tcp/tcp_trace.cc
)


set(redisnet_LIB
thirdparty/redisnet/ae.c
thirdparty/redisnet/anet.c
thirdparty/redisnet/zmalloc.c
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # require at least gcc 4.8
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC is too old, please install a newer version supporting C++11")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # require at least clang 3.3
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
        message(FATAL_ERROR "Clang is too old, please install a newer version supporting C++11")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang and GCC.")
endif()


macro(use_cxx11)
if(CMAKE_VERSION VERSION_LESS "3.1.3")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
endmacro(use_cxx11)

use_cxx11()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    #required by butil/crc32.cc to boost performance for 10x
    if((CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)") AND NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.4))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4 -msse4.2")
    elseif((CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64"))
        # segmentation fault in libcontext
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-gcse")
    endif()
    if(NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-aligned-new")
    endif()
endif()



# the reason why not using file(GLOB_RECURSE...) is that we want to
# include different files on different platforms.
set(BUTIL_SOURCES
    ${PROJECT_SOURCE_DIR}/butil/third_party/dmg_fp/g_fmt.cc
    ${PROJECT_SOURCE_DIR}/butil/third_party/dmg_fp/dtoa_wrapper.cc
    ${PROJECT_SOURCE_DIR}/butil/third_party/dynamic_annotations/dynamic_annotations.c
    ${PROJECT_SOURCE_DIR}/butil/third_party/icu/icu_utf.cc
    ${PROJECT_SOURCE_DIR}/butil/third_party/superfasthash/superfasthash.c
    ${PROJECT_SOURCE_DIR}/butil/third_party/modp_b64/modp_b64.cc
    ${PROJECT_SOURCE_DIR}/butil/third_party/symbolize/demangle.cc
    ${PROJECT_SOURCE_DIR}/butil/third_party/symbolize/symbolize.cc
    ${PROJECT_SOURCE_DIR}/butil/third_party/snappy/snappy-sinksource.cc
    ${PROJECT_SOURCE_DIR}/butil/third_party/snappy/snappy-stubs-internal.cc
    ${PROJECT_SOURCE_DIR}/butil/third_party/snappy/snappy.cc
    ${PROJECT_SOURCE_DIR}/butil/third_party/murmurhash3/murmurhash3.cpp
    ${PROJECT_SOURCE_DIR}/butil/arena.cpp
    ${PROJECT_SOURCE_DIR}/butil/at_exit.cc
    ${PROJECT_SOURCE_DIR}/butil/atomicops_internals_x86_gcc.cc
    ${PROJECT_SOURCE_DIR}/butil/base64.cc
    ${PROJECT_SOURCE_DIR}/butil/big_endian.cc
    ${PROJECT_SOURCE_DIR}/butil/cpu.cc
    ${PROJECT_SOURCE_DIR}/butil/debug/alias.cc
    ${PROJECT_SOURCE_DIR}/butil/debug/asan_invalid_access.cc
    ${PROJECT_SOURCE_DIR}/butil/debug/crash_logging.cc
    ${PROJECT_SOURCE_DIR}/butil/debug/debugger.cc
    ${PROJECT_SOURCE_DIR}/butil/debug/debugger_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/debug/dump_without_crashing.cc
    ${PROJECT_SOURCE_DIR}/butil/debug/proc_maps_linux.cc
    ${PROJECT_SOURCE_DIR}/butil/debug/stack_trace.cc
    ${PROJECT_SOURCE_DIR}/butil/debug/stack_trace_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/environment.cc
    ${PROJECT_SOURCE_DIR}/butil/files/file.cc
    ${PROJECT_SOURCE_DIR}/butil/files/file_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/files/file_enumerator.cc
    ${PROJECT_SOURCE_DIR}/butil/files/file_enumerator_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/files/file_path.cc
    ${PROJECT_SOURCE_DIR}/butil/files/file_path_constants.cc
    ${PROJECT_SOURCE_DIR}/butil/files/memory_mapped_file.cc
    ${PROJECT_SOURCE_DIR}/butil/files/memory_mapped_file_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/files/scoped_file.cc
    ${PROJECT_SOURCE_DIR}/butil/files/scoped_temp_dir.cc
    ${PROJECT_SOURCE_DIR}/butil/file_util.cc
    ${PROJECT_SOURCE_DIR}/butil/file_util_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/guid.cc
    ${PROJECT_SOURCE_DIR}/butil/guid_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/hash.cc
    ${PROJECT_SOURCE_DIR}/butil/lazy_instance.cc
    ${PROJECT_SOURCE_DIR}/butil/location.cc
    ${PROJECT_SOURCE_DIR}/butil/memory/aligned_memory.cc
    ${PROJECT_SOURCE_DIR}/butil/memory/ref_counted.cc
    ${PROJECT_SOURCE_DIR}/butil/memory/ref_counted_memory.cc
    ${PROJECT_SOURCE_DIR}/butil/memory/singleton.cc
    ${PROJECT_SOURCE_DIR}/butil/memory/weak_ptr.cc
    ${PROJECT_SOURCE_DIR}/butil/posix/file_descriptor_shuffle.cc
    ${PROJECT_SOURCE_DIR}/butil/posix/global_descriptors.cc
    ${PROJECT_SOURCE_DIR}/butil/process_util.cc
    ${PROJECT_SOURCE_DIR}/butil/rand_util.cc
    ${PROJECT_SOURCE_DIR}/butil/rand_util_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/fast_rand.cpp
    ${PROJECT_SOURCE_DIR}/butil/safe_strerror_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/sha1_portable.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/latin1_string_conversions.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/nullable_string16.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/safe_sprintf.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/string16.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/string_number_conversions.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/string_split.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/string_piece.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/string_util.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/string_util_constants.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/stringprintf.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/utf_offset_string_conversions.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/utf_string_conversion_utils.cc
    ${PROJECT_SOURCE_DIR}/butil/strings/utf_string_conversions.cc
    ${PROJECT_SOURCE_DIR}/butil/synchronization/cancellation_flag.cc
    ${PROJECT_SOURCE_DIR}/butil/synchronization/condition_variable_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/synchronization/waitable_event_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/non_thread_safe_impl.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/platform_thread_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/simple_thread.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/thread_checker_impl.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/thread_collision_warner.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/thread_id_name_manager.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/thread_local_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/thread_local_storage.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/thread_local_storage_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/thread_restrictions.cc
    ${PROJECT_SOURCE_DIR}/butil/threading/watchdog.cc
    ${PROJECT_SOURCE_DIR}/butil/time/clock.cc
    ${PROJECT_SOURCE_DIR}/butil/time/default_clock.cc
    ${PROJECT_SOURCE_DIR}/butil/time/default_tick_clock.cc
    ${PROJECT_SOURCE_DIR}/butil/time/tick_clock.cc
    ${PROJECT_SOURCE_DIR}/butil/time/time.cc
    ${PROJECT_SOURCE_DIR}/butil/time/time_posix.cc
    ${PROJECT_SOURCE_DIR}/butil/version.cc
    ${PROJECT_SOURCE_DIR}/butil/logging.cc
    ${PROJECT_SOURCE_DIR}/butil/class_name.cpp
    ${PROJECT_SOURCE_DIR}/butil/errno.cpp
    ${PROJECT_SOURCE_DIR}/butil/find_cstr.cpp
    ${PROJECT_SOURCE_DIR}/butil/status.cpp
    ${PROJECT_SOURCE_DIR}/butil/string_printf.cpp
    ${PROJECT_SOURCE_DIR}/butil/thread_local.cpp
    ${PROJECT_SOURCE_DIR}/butil/unix_socket.cpp
    ${PROJECT_SOURCE_DIR}/butil/endpoint.cpp
    ${PROJECT_SOURCE_DIR}/butil/fd_utility.cpp
    ${PROJECT_SOURCE_DIR}/butil/files/temp_file.cpp
    ${PROJECT_SOURCE_DIR}/butil/files/file_watcher.cpp
    ${PROJECT_SOURCE_DIR}/butil/time.cpp
    #${PROJECT_SOURCE_DIR}/butil/zero_copy_stream_as_streambuf.cpp
    ${PROJECT_SOURCE_DIR}/butil/crc32c.cc
    ${PROJECT_SOURCE_DIR}/butil/containers/case_ignored_flat_map.cpp
    #${PROJECT_SOURCE_DIR}/butil/iobuf.cpp
    #${PROJECT_SOURCE_DIR}/butil/binary_printer.cpp
    #${PROJECT_SOURCE_DIR}/butil/recordio.cc
    ${PROJECT_SOURCE_DIR}/butil/popen.cpp
    )

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(BUTIL_SOURCES ${BUTIL_SOURCES}
        ${PROJECT_SOURCE_DIR}/butil/file_util_linux.cc
        ${PROJECT_SOURCE_DIR}/butil/threading/platform_thread_linux.cc
        ${PROJECT_SOURCE_DIR}/butil/strings/sys_string_conversions_posix.cc)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(BUTIL_SOURCES ${BUTIL_SOURCES}
        ${PROJECT_SOURCE_DIR}/butil/mac/bundle_locations.mm
        ${PROJECT_SOURCE_DIR}/butil/mac/foundation_util.mm
        ${PROJECT_SOURCE_DIR}/butil/file_util_mac.mm
        ${PROJECT_SOURCE_DIR}/butil/threading/platform_thread_mac.mm
        ${PROJECT_SOURCE_DIR}/butil/strings/sys_string_conversions_mac.mm
        ${PROJECT_SOURCE_DIR}/butil/time/time_mac.cc
        ${PROJECT_SOURCE_DIR}/butil/mac/scoped_mach_port.cc)
endif()

add_library(tcp STATIC ${tcp_LIB})
add_library(base STATIC ${base_LIB})
add_library(net STATIC ${net_LIB})
add_library(butil STATIC ${BUTIL_SOURCES})
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
target_link_libraries(${EXECUTABLE_NAME}  net base butil pthread)


