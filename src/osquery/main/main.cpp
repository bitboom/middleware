/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#ifdef WIN32
#include <io.h>
#endif

#include <iostream>

#include <boost/algorithm/string/predicate.hpp>

#include <osquery/core.h>
#include <osquery/devtools/devtools.h>
#include <osquery/filesystem/fileops.h>
#include <osquery/logger.h>
#include <osquery/main/main.h>
#include <osquery/registry_factory.h>
#include <osquery/sql/sqlite_util.h>
#include <osquery/system.h>

//#include <osquery/experimental/tracing/syscalls_tracing.h>

namespace fs = boost::filesystem;

namespace osquery {

int startDaemon(Initializer& runner) {
  runner.start();

//  osquery::events::init_syscall_tracing();

  // Finally wait for a signal / interrupt to shutdown.
  runner.waitForShutdown();
  return 0;
}

int startShell(osquery::Initializer& runner, int argc, char* argv[]) {
  int retcode = 0;

  // Finally shutdown.
  runner.requestShutdown();
  return retcode;
}

int startOsquery(int argc, char* argv[], std::function<void()> shutdown) {
  // Parse/apply flags, start registry, load logger/config plugins.
  osquery::Initializer runner(argc, argv, osquery::ToolType::SHELL);

  runner.installShutdown(shutdown);
  runner.initDaemon();

  if (runner.isDaemon()) {
    return startDaemon(runner);
  }
  return startShell(runner, argc, argv);
}
} // namespace osquery
