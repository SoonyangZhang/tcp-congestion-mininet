#pragma once
#include <utility>

#include "base/optional.h"

namespace quiche {
template <typename T>
using QuicheOptionalImpl = nonstd::optional<T>;

#define QUICHE_NULLOPT_IMPL nonstd::nullopt

template <typename T>
using QuicheOptional = QuicheOptionalImpl<T>;

#define QUICHE_NULLOPT QUICHE_NULLOPT_IMPL

}  // namespace quiche
