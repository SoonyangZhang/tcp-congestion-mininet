// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef QUICHE_COMMON_PLATFORM_API_QUICHE_STRING_PIECE_H_
#define QUICHE_COMMON_PLATFORM_API_QUICHE_STRING_PIECE_H_

#include "butil/strings/string_piece.h"

namespace quiche {
using QuicheStringPieceImpl = butil::StringPiece;

using QuicheStringPieceHashImpl = butil::StringPieceHash;

inline size_t QuicheHashStringPairImpl(QuicheStringPieceImpl a,
                                       QuicheStringPieceImpl b) {
  return butil::StringPieceHash()(a) ^ butil::StringPieceHash()(b);
}

using QuicheStringPiece = QuicheStringPieceImpl;

using QuicheStringPieceHash = QuicheStringPieceHashImpl;

inline size_t QuicheHashStringPair(QuicheStringPiece a, QuicheStringPiece b) {
  return QuicheHashStringPairImpl(a, b);
}

}  // namespace quiche

#endif  // QUICHE_COMMON_PLATFORM_API_QUICHE_STRING_PIECE_H_
