/**
 *  Copyright (c) 2018-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#pragma once

#include <mutex>
#include <shared_mutex>
#include <boost/thread/recursive_mutex.hpp>

namespace osquery {

/// Helper alias for defining mutexes.
using Mutex = std::shared_timed_mutex;

/// Helper alias for write locking a mutex.
using WriteLock = std::unique_lock<Mutex>;

/// Helper alias for read locking a mutex.
using ReadLock = std::shared_lock<Mutex>;

/// Helper alias for defining recursive mutexes.
using RecursiveMutex = std::recursive_mutex;

/// Helper alias for write locking a recursive mutex.
using RecursiveLock = std::unique_lock<std::recursive_mutex>;

/// Helper alias for upgrade locking a mutex.
using UpgradeLock = boost::upgrade_lock<Mutex>;

/// Helper alias for write locking an upgrade lock.
using WriteUpgradeLock = boost::upgrade_to_unique_lock<Mutex>;

} // namespace osquery
