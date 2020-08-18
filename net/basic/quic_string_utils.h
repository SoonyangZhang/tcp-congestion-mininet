#pragma once
#include <stdarg.h>
#include <string>
#include <iostream>
#include <sstream>
namespace quic{
inline std::string PoormanStringPrintf(const char *format,...){
    size_t i=0;
    char buf[512]={'\0'};
    va_list ap;
    va_start(ap, format);
    i=vsnprintf(buf,512,format,ap);
    va_end(ap);
    return std::string(buf,i);
}
template <typename... Args>
inline std::string QuicheStrCatImpl(const Args&... args) {
  std::ostringstream oss;
  int dummy[] = {1, (oss << args, 0)...};
  static_cast<void>(dummy);
  return oss.str();
}
template <typename... Args>
inline std::string QuicheStringPrintfImpl(const Args&... args) {
  return PoormanStringPrintf(std::forward<const Args&>(args)...);
}
template <typename... Args>
inline std::string QuicheStrCat(const Args&... args) {
  return QuicheStrCatImpl(std::forward<const Args&>(args)...);
}

template <typename... Args>
inline std::string QuicheStringPrintf(const Args&... args) {
  return QuicheStringPrintfImpl(std::forward<const Args&>(args)...);
}
}
