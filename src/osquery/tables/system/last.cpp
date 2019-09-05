/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <vector>
#include <string>

#include <utmpx.h>

#include <osquery/core.h>
#include <osquery/tables.h>

namespace osquery {
namespace tables {

QueryData genLastAccess(QueryContext& context) {
  QueryData results;
  struct utmpx *ut;
#ifdef __APPLE__
  setutxent_wtmp(0); // 0 = reverse chronological order

  while ((ut = getutxent_wtmp()) != nullptr) {
#else

  utmpxname("/var/log/wtmpx");
  setutxent();

  while ((ut = getutxent()) != nullptr) {
#endif

    Row r;
    r["username"] = std::string(ut->ut_user);
    r["tty"] = std::string(ut->ut_line);
    r["pid"] = boost::lexical_cast<std::string>(ut->ut_pid);
    r["type"] = boost::lexical_cast<std::string>(ut->ut_type);
    r["time"] = boost::lexical_cast<std::string>(ut->ut_tv.tv_sec);
    r["host"] = std::string(ut->ut_host);

    results.push_back(r);
  }

#ifdef __APPLE__
  endutxent_wtmp();
#else
  endutxent();
#endif

  return results;
}
}
}
