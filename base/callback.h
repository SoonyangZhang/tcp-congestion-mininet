#pragma once
#include <string>
#include "atomic_ops.h"
#include "scoped_refptr.h"
//copy from ns3 callback
namespace base{
template <typename T>
struct CallbackTraits;

template <typename T>
struct CallbackTraits<T *>
{
  static T & GetReference (T * const p)
  {
    return *p;
  }
};
class CallbackImplBase : public SimpleRefCount<CallbackImplBase>
{
public:
  virtual ~CallbackImplBase () {}
  virtual bool IsEqual (scoped_refptr<const CallbackImplBase> other) const = 0;
  virtual std::string GetTypeid (void) const = 0;

protected:
  static std::string Demangle (const std::string& mangled);
  template <typename T>
  static std::string GetCppTypeid (void)
  {
    std::string typeName;
    try
      {
        typeName = typeid (T).name ();
        typeName = Demangle (typeName);
      }
    catch (const std::bad_typeid &e)
      {
        typeName = e.what ();
      }
    return typeName;
  }
};
template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
class CallbackImpl;

template <typename R>
class CallbackImpl<R,empty,empty,empty,empty,empty,empty,empty,empty,empty> : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (void) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () +
      ">";
    return id;
  }
};
template <typename R, typename T1>
class CallbackImpl<R,T1,empty,empty,empty,empty,empty,empty,empty,empty> : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (T1) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () + "," +
      GetCppTypeid<T1> () +
      ">";
    return id;
  }
};
template <typename R, typename T1, typename T2>
class CallbackImpl<R,T1,T2,empty,empty,empty,empty,empty,empty,empty> : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (T1, T2) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () + "," +
      GetCppTypeid<T1> () + "," +
      GetCppTypeid<T2> () +
      ">";
    return id;
  }
};
template <typename R, typename T1, typename T2, typename T3>
class CallbackImpl<R,T1,T2,T3,empty,empty,empty,empty,empty,empty> : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (T1, T2, T3) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () + "," +
      GetCppTypeid<T1> () + "," +
      GetCppTypeid<T2> () + "," +
      GetCppTypeid<T3> () +
      ">";
    return id;
  }
};
template <typename R, typename T1, typename T2, typename T3, typename T4>
class CallbackImpl<R,T1,T2,T3,T4,empty,empty,empty,empty,empty> : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (T1, T2, T3, T4) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () + "," +
      GetCppTypeid<T1> () + "," +
      GetCppTypeid<T2> () + "," +
      GetCppTypeid<T3> () + "," +
      GetCppTypeid<T4> () +
      ">";
    return id;
  }
};
template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
class CallbackImpl<R,T1,T2,T3,T4,T5,empty,empty,empty,empty> : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (T1, T2, T3, T4, T5) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () + "," +
      GetCppTypeid<T1> () + "," +
      GetCppTypeid<T2> () + "," +
      GetCppTypeid<T3> () + "," +
      GetCppTypeid<T4> () + "," +
      GetCppTypeid<T5> () +
      ">";
    return id;
  }
};
template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
class CallbackImpl<R,T1,T2,T3,T4,T5,T6,empty,empty,empty> : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (T1, T2, T3, T4, T5, T6) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () + "," +
      GetCppTypeid<T1> () + "," +
      GetCppTypeid<T2> () + "," +
      GetCppTypeid<T3> () + "," +
      GetCppTypeid<T4> () + "," +
      GetCppTypeid<T5> () + "," +
      GetCppTypeid<T6> () +
      ">";
    return id;
  }
};
template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
class CallbackImpl<R,T1,T2,T3,T4,T5,T6,T7,empty,empty> : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (T1, T2, T3, T4, T5, T6, T7) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () + "," +
      GetCppTypeid<T1> () + "," +
      GetCppTypeid<T2> () + "," +
      GetCppTypeid<T3> () + "," +
      GetCppTypeid<T4> () + "," +
      GetCppTypeid<T5> () + "," +
      GetCppTypeid<T6> () + "," +
      GetCppTypeid<T7> () +
      ">";
    return id;
  }
};
template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
class CallbackImpl<R,T1,T2,T3,T4,T5,T6,T7,T8,empty> : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (T1, T2, T3, T4, T5, T6, T7, T8) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () + "," +
      GetCppTypeid<T1> () + "," +
      GetCppTypeid<T2> () + "," +
      GetCppTypeid<T3> () + "," +
      GetCppTypeid<T4> () + "," +
      GetCppTypeid<T5> () + "," +
      GetCppTypeid<T6> () + "," +
      GetCppTypeid<T7> () + "," +
      GetCppTypeid<T8> () +
      ">";
    return id;
  }
};
template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
class CallbackImpl : public CallbackImplBase {
public:
  virtual ~CallbackImpl () {}
  virtual R operator() (T1, T2, T3, T4, T5, T6, T7, T8, T9) = 0;
  virtual std::string GetTypeid (void) const
  {
    return DoGetTypeid ();
  }
  static std::string DoGetTypeid (void)
  {
    static std::string id = "CallbackImpl<" +
      GetCppTypeid<R> () + "," +
      GetCppTypeid<T1> () + "," +
      GetCppTypeid<T2> () + "," +
      GetCppTypeid<T3> () + "," +
      GetCppTypeid<T4> () + "," +
      GetCppTypeid<T5> () + "," +
      GetCppTypeid<T6> () + "," +
      GetCppTypeid<T7> () + "," +
      GetCppTypeid<T8> () + "," +
      GetCppTypeid<T9> () +
      ">";
    return id;
  }
};
template <typename T, typename R, typename T1, typename T2, typename T3, typename T4,typename T5, typename T6, typename T7, typename T8, typename T9>
class FunctorCallbackImpl : public CallbackImpl<R,T1,T2,T3,T4,T5,T6,T7,T8,T9> {
public:
  FunctorCallbackImpl (T const &functor)
    : m_functor (functor) {}
  virtual ~FunctorCallbackImpl () {}
  R operator() (void) {
    return m_functor ();
  }
  R operator() (T1 a1) {
    return m_functor (a1);
  }
  R operator() (T1 a1,T2 a2) {
    return m_functor (a1,a2);
  }
  R operator() (T1 a1,T2 a2,T3 a3) {
    return m_functor (a1,a2,a3);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4) {
    return m_functor (a1,a2,a3,a4);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5) {
    return m_functor (a1,a2,a3,a4,a5);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5,T6 a6) {
    return m_functor (a1,a2,a3,a4,a5,a6);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5,T6 a6,T7 a7) {
    return m_functor (a1,a2,a3,a4,a5,a6,a7);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5,T6 a6,T7 a7,T8 a8) {
    return m_functor (a1,a2,a3,a4,a5,a6,a7,a8);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5,T6 a6,T7 a7,T8 a8,T9 a9) {
    return m_functor (a1,a2,a3,a4,a5,a6,a7,a8,a9);
  }
  virtual bool IsEqual (scoped_refptr<const CallbackImplBase> other) const {
    FunctorCallbackImpl<T,R,T1,T2,T3,T4,T5,T6,T7,T8,T9> const *otherDerived =
      dynamic_cast<FunctorCallbackImpl<T,R,T1,T2,T3,T4,T5,T6,T7,T8,T9> const *> (PeekPointer (other));
    if (otherDerived == 0)
      {
        return false;
      }
    else if (otherDerived->m_functor != m_functor)
      {
        return false;
      }
    return true;
  }
private:
  T m_functor;
};

template <typename OBJ_PTR, typename MEM_PTR, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
class MemPtrCallbackImpl : public CallbackImpl<R,T1,T2,T3,T4,T5,T6,T7,T8,T9> {
public:
  MemPtrCallbackImpl (OBJ_PTR const&objPtr, MEM_PTR memPtr)
    : m_objPtr (objPtr), m_memPtr (memPtr) {}
  virtual ~MemPtrCallbackImpl () {}
  R operator() (void) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)();
  }
  R operator() (T1 a1) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)(a1);
  }
  R operator() (T1 a1,T2 a2) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)(a1, a2);
  }
  R operator() (T1 a1,T2 a2,T3 a3) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)(a1, a2, a3);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)(a1, a2, a3, a4);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)(a1, a2, a3, a4, a5);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5,T6 a6) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)(a1, a2, a3, a4, a5, a6);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5,T6 a6,T7 a7) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)(a1, a2, a3, a4, a5, a6, a7);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5,T6 a6,T7 a7,T8 a8) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)(a1, a2, a3, a4, a5, a6, a7, a8);
  }
  R operator() (T1 a1,T2 a2,T3 a3,T4 a4,T5 a5,T6 a6,T7 a7,T8 a8, T9 a9) {
    return ((CallbackTraits<OBJ_PTR>::GetReference (m_objPtr)).*m_memPtr)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
  }
  virtual bool IsEqual (scoped_refptr<const CallbackImplBase> other) const {
    MemPtrCallbackImpl<OBJ_PTR,MEM_PTR,R,T1,T2,T3,T4,T5,T6,T7,T8,T9> const *otherDerived =
      dynamic_cast<MemPtrCallbackImpl<OBJ_PTR,MEM_PTR,R,T1,T2,T3,T4,T5,T6,T7,T8,T9> const *> (PeekPointer (other));
    if (otherDerived == 0)
      {
        return false;
      }
    else if (otherDerived->m_objPtr != m_objPtr ||
             otherDerived->m_memPtr != m_memPtr)
      {
        return false;
      }
    return true;
  }
private:
  OBJ_PTR const m_objPtr;
  MEM_PTR m_memPtr;
};
class CallbackBase {
public:
  CallbackBase () : m_impl () {}
  scoped_refptr<CallbackImplBase> GetImpl (void) const { return m_impl; }
protected:
  CallbackBase (scoped_refptr<CallbackImplBase> impl) : m_impl (impl) {}
  scoped_refptr<CallbackImplBase> m_impl;
};

template<typename R,
         typename T1 = empty, typename T2 = empty,
         typename T3 = empty, typename T4 = empty,
         typename T5 = empty, typename T6 = empty,
         typename T7 = empty, typename T8 = empty,
         typename T9 = empty>
class Callback : public CallbackBase {
public:
  Callback () {}

  template <typename FUNCTOR>
  Callback (FUNCTOR const &functor, bool, bool)
    : CallbackBase (Create<FunctorCallbackImpl<FUNCTOR,R,T1,T2,T3,T4,T5,T6,T7,T8,T9> > (functor))
  {}

  template <typename OBJ_PTR, typename MEM_PTR>
  Callback (OBJ_PTR const &objPtr, MEM_PTR memPtr)
    : CallbackBase (Create<MemPtrCallbackImpl<OBJ_PTR,MEM_PTR,R,T1,T2,T3,T4,T5,T6,T7,T8,T9> > (objPtr, memPtr))
  {}

  Callback (scoped_refptr<CallbackImpl<R,T1,T2,T3,T4,T5,T6,T7,T8,T9> > const &impl)
    : CallbackBase (impl)
  {}
  bool IsNull (void) const {
    return (DoPeekImpl () == 0) ? true : false;
  }
  void Nullify (void) {
    m_impl = 0;
  }

  R operator() (void) const {
    return (*(DoPeekImpl ()))();
  }
  R operator() (T1 a1) const {
    return (*(DoPeekImpl ()))(a1);
  }
  R operator() (T1 a1, T2 a2) const {
    return (*(DoPeekImpl ()))(a1,a2);
  }
  R operator() (T1 a1, T2 a2, T3 a3) const {
    return (*(DoPeekImpl ()))(a1,a2,a3);
  }
  R operator() (T1 a1, T2 a2, T3 a3, T4 a4) const {
    return (*(DoPeekImpl ()))(a1,a2,a3,a4);
  }
  R operator() (T1 a1, T2 a2, T3 a3, T4 a4,T5 a5) const {
    return (*(DoPeekImpl ()))(a1,a2,a3,a4,a5);
  }
  R operator() (T1 a1, T2 a2, T3 a3, T4 a4,T5 a5,T6 a6) const {
    return (*(DoPeekImpl ()))(a1,a2,a3,a4,a5,a6);
  }
  R operator() (T1 a1, T2 a2, T3 a3, T4 a4,T5 a5,T6 a6,T7 a7) const {
    return (*(DoPeekImpl ()))(a1,a2,a3,a4,a5,a6,a7);
  }
  R operator() (T1 a1, T2 a2, T3 a3, T4 a4,T5 a5,T6 a6,T7 a7,T8 a8) const {
    return (*(DoPeekImpl ()))(a1,a2,a3,a4,a5,a6,a7,a8);
  }
  R operator() (T1 a1, T2 a2, T3 a3, T4 a4,T5 a5,T6 a6,T7 a7,T8 a8, T9 a9) const {
    return (*(DoPeekImpl ()))(a1,a2,a3,a4,a5,a6,a7,a8,a9);
  }
  bool IsEqual (const CallbackBase &other) const {
    return m_impl->IsEqual (other.GetImpl ());
  }

  bool CheckType (const CallbackBase & other) const {
    return DoCheckType (other.GetImpl ());
  }
  bool Assign (const CallbackBase &other) {
    return DoAssign (other.GetImpl ());
  }
private:
  CallbackImpl<R,T1,T2,T3,T4,T5,T6,T7,T8,T9> *DoPeekImpl (void) const {
    return static_cast<CallbackImpl<R,T1,T2,T3,T4,T5,T6,T7,T8,T9> *> (PeekPointer(m_impl));
  }
  bool DoCheckType (scoped_refptr<const CallbackImplBase> other) const {
    if (other != 0 &&
        dynamic_cast<const CallbackImpl<R,T1,T2,T3,T4,T5,T6,T7,T8,T9> *> (PeekPointer (other)) != 0)
      {
        return true;
      }
    else if (other == 0)
      {
        return true;
      }
    else
      {
        return false;
      }
  }
  bool DoAssign (scoped_refptr<const CallbackImplBase> other) {
    if (!DoCheckType (other))
      {
        std::string othTid = other->GetTypeid ();
        std::string myTid = CallbackImpl<R,T1,T2,T3,T4,T5,T6,T7,T8,T9>::DoGetTypeid ();
       // NS_FATAL_ERROR_CONT ("Incompatible types. (feed to \"c++filt -t\" if needed)" << std::endl <<
        //                "got=" << othTid << std::endl <<
        //                "expected=" << myTid);
        return false;
      }
    m_impl = const_cast<CallbackImplBase *> (PeekPointer (other));
    return true;
  }
};
template <typename T, typename OBJ, typename R>
Callback<R> MakeCallback (R (T::*memPtr)(void), OBJ objPtr) {
  return Callback<R> (objPtr, memPtr);
}
template <typename T, typename OBJ, typename R>
Callback<R> MakeCallback (R (T::*memPtr)() const, OBJ objPtr) {
  return Callback<R> (objPtr, memPtr);
}
template <typename T, typename OBJ, typename R, typename T1>
Callback<R,T1> MakeCallback (R (T::*memPtr)(T1), OBJ objPtr) {
  return Callback<R,T1> (objPtr, memPtr);
}
template <typename T, typename OBJ, typename R, typename T1>
Callback<R,T1> MakeCallback (R (T::*memPtr)(T1) const, OBJ objPtr) {
  return Callback<R,T1> (objPtr, memPtr);
}
template <typename T, typename OBJ, typename R, typename T1, typename T2>
Callback<R,T1,T2> MakeCallback (R (T::*memPtr)(T1,T2), OBJ objPtr) {
  return Callback<R,T1,T2> (objPtr, memPtr);
}
template <typename T, typename OBJ, typename R, typename T1, typename T2>
Callback<R,T1,T2> MakeCallback (R (T::*memPtr)(T1,T2) const, OBJ objPtr) {
  return Callback<R,T1,T2> (objPtr, memPtr);
}
template <typename T, typename OBJ, typename R, typename T1,typename T2, typename T3>
Callback<R,T1,T2,T3> MakeCallback (R (T::*memPtr)(T1,T2,T3), OBJ objPtr) {
  return Callback<R,T1,T2,T3> (objPtr, memPtr);
}
}