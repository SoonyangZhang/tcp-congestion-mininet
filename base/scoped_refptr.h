#pragma once
#include "atomic_ops.h"
#include <memory>
#include <utility>
namespace base{
template <class T>
class scoped_refptr {
 public:
  typedef T element_type;

  scoped_refptr() : ptr_(nullptr) {}

  scoped_refptr(T* p) : ptr_(p) {  // NOLINT(runtime/explicit)
    if (ptr_)
      ptr_->AddRef();
  }

  scoped_refptr(const scoped_refptr<T>& r) : ptr_(r.ptr_) {
    if (ptr_)
      ptr_->AddRef();
  }

  template <typename U>
  scoped_refptr(const scoped_refptr<U>& r) : ptr_(r.get()) {
    if (ptr_)
      ptr_->AddRef();
  }

  // Move constructors.
  scoped_refptr(scoped_refptr<T>&& r) : ptr_(r.release()) {}

  template <typename U>
  scoped_refptr(scoped_refptr<U>&& r) : ptr_(r.release()) {}

  ~scoped_refptr() {
    if (ptr_)
      ptr_->Release();
  }

  T* get() const { return ptr_; }
  operator T*() const { return ptr_; }
  T* operator->() const { return ptr_; }

  // Returns the (possibly null) raw pointer, and makes the scoped_refptr hold a
  // null pointer, all without touching the reference count of the underlying
  // pointed-to object. The object is still reference counted, and the caller of
  // release() is now the proud owner of one reference, so it is responsible for
  // calling Release() once on the object when no longer using it.
  T* release() {
    T* retVal = ptr_;
    ptr_ = nullptr;
    return retVal;
  }

  scoped_refptr<T>& operator=(T* p) {
    // AddRef first so that self assignment should work
    if (p)
      p->AddRef();
    if (ptr_)
      ptr_->Release();
    ptr_ = p;
    return *this;
  }

  scoped_refptr<T>& operator=(const scoped_refptr<T>& r) {
    return *this = r.ptr_;
  }

  template <typename U>
  scoped_refptr<T>& operator=(const scoped_refptr<U>& r) {
    return *this = r.get();
  }

  scoped_refptr<T>& operator=(scoped_refptr<T>&& r) {
    scoped_refptr<T>(std::move(r)).swap(*this);
    return *this;
  }

  template <typename U>
  scoped_refptr<T>& operator=(scoped_refptr<U>&& r) {
    scoped_refptr<T>(std::move(r)).swap(*this);
    return *this;
  }

  void swap(T** pp) {
    T* p = ptr_;
    ptr_ = *pp;
    *pp = p;
  }

void swap(scoped_refptr<T>& r) { swap(&r.ptr_); }
template <typename U>
friend U *GetPointer (const scoped_refptr<U> &p);
template <typename U>
friend U *PeekPointer (const scoped_refptr<U> &p);
protected:
T* ptr_;
};
template <typename U>
U * PeekPointer (const scoped_refptr<U> &p)
{
return p.get();
}
template <typename T>
scoped_refptr<T> Create (void)
{
   return scoped_refptr<T> (new T ());
}
template <typename T, typename T1>
scoped_refptr<T> Create (T1 a1)
{
   return scoped_refptr<T> (new T (a1));
}
template <typename T, typename T1,typename T2>
scoped_refptr<T> Create (T1 a1,T2 a2)
{
   return scoped_refptr<T> (new T (a1,a2));
}
}