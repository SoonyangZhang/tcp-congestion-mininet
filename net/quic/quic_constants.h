#pragma once
namespace quic {
using QuicByteCount = uint64_t;
// Simple time constants.
const uint64_t kNumSecondsPerMinute = 60;
const uint64_t kNumSecondsPerHour = kNumSecondsPerMinute * 60;
const uint64_t kNumSecondsPerWeek = kNumSecondsPerHour * 24 * 7;
const uint64_t kNumMillisPerSecond = 1000;
const uint64_t kNumMicrosPerMilli = 1000;
const uint64_t kNumMicrosPerSecond = kNumMicrosPerMilli * kNumMillisPerSecond;
}
