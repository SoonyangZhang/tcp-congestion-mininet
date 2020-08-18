/*
 *  Copyright 2011 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


#pragma once
#include <utility>
#include "magic.h"
#if defined(WIN_32)
// clang-format off
// clang formating would change include order.

// Include winsock2.h before including <windows.h> to maintain consistency with
// win32.h. To include win32.h directly, it must be broken out into its own
// build target.
#include <winsock2.h>
#include <windows.h>
// clang-format on
#endif  // defined(WEBRTC_WIN)

namespace base{
class AtomicOps {
 public:
#if defined(WIN_32)
  // Assumes sizeof(int) == sizeof(LONG), which it is on Win32 and Win64.
  static int Increment(volatile int* i) {
    return ::InterlockedIncrement(reinterpret_cast<volatile LONG*>(i));
  }
  static int Decrement(volatile int* i) {
    return ::InterlockedDecrement(reinterpret_cast<volatile LONG*>(i));
  }
  static int AcquireLoad(volatile const int* i) { return *i; }
  static void ReleaseStore(volatile int* i, int value) { *i = value; }
  static int CompareAndSwap(volatile int* i, int old_value, int new_value) {
    return ::InterlockedCompareExchange(reinterpret_cast<volatile LONG*>(i),
                                        new_value, old_value);
  }
  // Pointer variants.
  template <typename T>
  static T* AcquireLoadPtr(T* volatile* ptr) {
    return *ptr;
  }
  template <typename T>
  static T* CompareAndSwapPtr(T* volatile* ptr, T* old_value, T* new_value) {
    return static_cast<T*>(::InterlockedCompareExchangePointer(
        reinterpret_cast<PVOID volatile*>(ptr), new_value, old_value));
  }
#else
  static int Increment(volatile int* i) { return __sync_add_and_fetch(i, 1); }
  static int Decrement(volatile int* i) { return __sync_sub_and_fetch(i, 1); }
  static int AcquireLoad(volatile const int* i) {
    return __atomic_load_n(i, __ATOMIC_ACQUIRE);
  }
  static void ReleaseStore(volatile int* i, int value) {
    __atomic_store_n(i, value, __ATOMIC_RELEASE);
  }
  static int CompareAndSwap(volatile int* i, int old_value, int new_value) {
    return __sync_val_compare_and_swap(i, old_value, new_value);
  }
  // Pointer variants.
  template <typename T>
  static T* AcquireLoadPtr(T* volatile* ptr) {
    return __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
  }
  template <typename T>
  static T* CompareAndSwapPtr(T* volatile* ptr, T* old_value, T* new_value) {
    return __sync_val_compare_and_swap(ptr, old_value, new_value);
  }
#endif
};
enum class RefCountReleaseStatus { kDroppedLastRef, kOtherRefsRemained };

// Interfaces where refcounting is part of the public api should
// inherit this abstract interface. The implementation of these
// methods is usually provided by the RefCountedObject template class,
// applied as a leaf in the inheritance tree.
class RefCountInterface {
 public:
  virtual void AddRef() const = 0;
  virtual RefCountReleaseStatus Release() const = 0;

  // Non-public destructor, because Release() has exclusive responsibility for
  // destroying the object.
 protected:
  virtual ~RefCountInterface() {}
};
class RefCounter {
 public:
  explicit RefCounter(int ref_count) : ref_count_(ref_count) {}
  RefCounter() = delete;

  void IncRef() { AtomicOps::Increment(&ref_count_); }

  // Returns kDroppedLastRef if this call dropped the last reference; the caller
  // should therefore free the resource protected by the reference counter.
  // Otherwise, returns kOtherRefsRemained (note that in case of multithreading,
  // some other caller may have dropped the last reference by the time this call
  // returns; all we know is that we didn't do it).
  RefCountReleaseStatus DecRef() {
    return (AtomicOps::Decrement(&ref_count_) == 0)
               ? RefCountReleaseStatus::kDroppedLastRef
               : RefCountReleaseStatus::kOtherRefsRemained;
  }

  // Return whether the reference count is one. If the reference count is used
  // in the conventional way, a reference count of 1 implies that the current
  // thread owns the reference and no other thread shares it. This call performs
  // the test for a reference count of one, and performs the memory barrier
  // needed for the owning thread to act on the resource protected by the
  // reference counter, knowing that it has exclusive access.
  bool HasOneRef() const {
    return AtomicOps::AcquireLoad(&ref_count_) == 1;
  }
  int GetRefCount() const{
      return ref_count_;
  }
 private:
  volatile int ref_count_;
};
class RefCountedBase {
 public:
  RefCountedBase() = default;

  void AddRef() const { ref_count_.IncRef(); }
  RefCountReleaseStatus Release() const {
    const auto status = ref_count_.DecRef();
    if (status == RefCountReleaseStatus::kDroppedLastRef) {
      delete this;
    }
    return status;
  }

 protected:
  virtual ~RefCountedBase() = default;

 private:
  mutable RefCounter ref_count_{0};

  DISALLOW_COPY_AND_ASSIGN(RefCountedBase);
};
template <class T>
class RefCountedObject : public T {
 public:
  RefCountedObject() {}

  template <class P0>
  explicit RefCountedObject(P0&& p0) : T(std::forward<P0>(p0)) {}

  template <class P0, class P1, class... Args>
  RefCountedObject(P0&& p0, P1&& p1, Args&&... args)
      : T(std::forward<P0>(p0),
          std::forward<P1>(p1),
          std::forward<Args>(args)...) {}

  virtual void AddRef() const { ref_count_.IncRef(); }

  virtual RefCountReleaseStatus Release() const {
    const auto status = ref_count_.DecRef();
    if (status == RefCountReleaseStatus::kDroppedLastRef) {
      delete this;
    }
    return status;
  }

  // Return whether the reference count is one. If the reference count is used
  // in the conventional way, a reference count of 1 implies that the current
  // thread owns the reference and no other thread shares it. This call
  // performs the test for a reference count of one, and performs the memory
  // barrier needed for the owning thread to act on the object, knowing that it
  // has exclusive access to the object.
  virtual bool HasOneRef() const { return ref_count_.HasOneRef(); }
  int GetRefCount(){
      return ref_count_.GetRefCount();
  }
 protected:
  virtual ~RefCountedObject() {}

  mutable RefCounter ref_count_{0};

  DISALLOW_COPY_AND_ASSIGN(RefCountedObject);
};
class empty{};
template<typename T,typename Parent=empty>
class SimpleRefCount:public Parent{
public:
    SimpleRefCount(){}
  virtual void AddRef() const { ref_count_.IncRef(); }

  virtual RefCountReleaseStatus Release() const {
    const auto status = ref_count_.DecRef();
    if (status == RefCountReleaseStatus::kDroppedLastRef) {
      delete static_cast<T*> (const_cast<SimpleRefCount *> (this));
    }
    return status;
  }
  int GetRefCount(){
      return ref_count_.GetRefCount();
  }
protected:
    virtual ~SimpleRefCount(){}
    mutable RefCounter ref_count_{0};
    DISALLOW_COPY_AND_ASSIGN(SimpleRefCount);
};
}
