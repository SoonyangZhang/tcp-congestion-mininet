#pragma once

#ifdef _MSC_VER
// MSVC 2013 and prior don't have alignof or aligned(); they have __alignof and
// a __declspec instead.
#define QUIC_ALIGN_OF_IMPL __alignof
#define QUIC_ALIGNED_IMPL(X) __declspec(align(X))
#else
#define QUIC_ALIGN_OF_IMPL alignof
#define QUIC_ALIGNED_IMPL(X) __attribute__((aligned(X)))
#endif  // _MSC_VER

// TODO(rtenneti): Change the default 64 alignas value (used the default
// value from ABSL_CACHELINE_SIZE).
#define QUIC_CACHELINE_SIZE_IMPL (64)
#define QUIC_CACHELINE_ALIGNED_IMPL ALIGNAS(QUIC_CACHELINE_SIZE)

#define QUIC_ALIGN_OF QUIC_ALIGN_OF_IMPL
#define QUIC_ALIGNED(X) QUIC_ALIGNED_IMPL(X)
#define QUIC_CACHELINE_ALIGNED QUIC_CACHELINE_ALIGNED_IMPL
#define QUIC_CACHELINE_SIZE QUIC_CACHELINE_SIZE_IMPL