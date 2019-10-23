#include "callback.h"
#include <cassert>
 #if (__GNUC__ >= 3)

 #include <cstdlib>
 #include <cxxabi.h>
 namespace base{

 std::string
 CallbackImplBase::Demangle (const std::string& mangled)
 {
   //NS_LOG_FUNCTION (mangled);

   int status;
   char* demangled = abi::__cxa_demangle (mangled.c_str (),
                                          NULL, NULL, &status);

   std::string ret;
   if (status == 0) {
       assert (demangled);
       ret = demangled;
     }
   else if (status == -1) {
       //NS_LOG_UNCOND ("Callback demangling failed: Memory allocation failure occurred.");
       ret = mangled;
     }
   else if (status == -2) {
       //NS_LOG_UNCOND ("Callback demangling failed: Mangled name is not a valid under the C++ ABI mangling rules.");
       ret = mangled;
     }
   else if (status == -3) {
      // NS_LOG_UNCOND ("Callback demangling failed: One of the arguments is invalid.");
       ret = mangled;
     }
   else {
       //NS_LOG_UNCOND ("Callback demangling failed: status " << status);
       ret = mangled;
     }

   if (demangled) {
       std::free (demangled);
     }
   return ret;
 }

 } // namespace ns3

 #else

 std::string
 base::CallbackImplBase::Demangle (const std::string& mangled)
 {
   return mangled;
 }

 #endif