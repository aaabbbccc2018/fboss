/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "fboss/agent/hw/sai/api/FdbApi.h"

#include <boost/functional/hash.hpp>

#include <functional>

namespace std {
size_t hash<facebook::fboss::FdbApiParameters::FdbEntry>::operator()(
    const facebook::fboss::FdbApiParameters::FdbEntry& fdbEntry) const {
  size_t seed = 0;
  boost::hash_combine(seed, fdbEntry.switchId());
  boost::hash_combine(seed, fdbEntry.bridgeId());
  boost::hash_combine(seed, std::hash<folly::MacAddress>()(fdbEntry.mac()));
  return seed;
}
} // namespace std
