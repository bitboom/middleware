/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include "diff_results.h"

namespace osquery {

DiffResults diff(QueryDataSet& old, QueryDataTyped& current) {
  DiffResults r;

  for (auto& i : current) {
    auto item = old.find(i);
    if (item != old.end()) {
      old.erase(item);
    } else {
      r.added.push_back(i);
    }
  }

  for (auto& i : old) {
    r.removed.push_back(std::move(i));
  }

  return r;
}

} // namespace osquery
