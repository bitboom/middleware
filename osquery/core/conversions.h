/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#pragma once

#include <memory>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

namespace osquery {

template <typename T>
void do_release_boost(typename boost::shared_ptr<T> const&, T*) {}

/**
 * @brief Convert a boost::shared_ptr to a std::shared_ptr
 */
template <typename T>
typename std::shared_ptr<T> boost_to_std_shared_ptr(
    typename boost::shared_ptr<T> const& p) {
  return std::shared_ptr<T>(p.get(), boost::bind(&do_release_boost<T>, p, _1));
}

template <typename T>
void do_release_std(typename std::shared_ptr<T> const&, T*) {}

/**
 * @brief Convert a std::shared_ptr to a boost::shared_ptr
 */
template <typename T>
typename boost::shared_ptr<T> std_to_boost_shared_ptr(
    typename std::shared_ptr<T> const& p) {
  return boost::shared_ptr<T>(p.get(), boost::bind(&do_release_std<T>, p, _1));
}
}
