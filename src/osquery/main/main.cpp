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
#include <osquery/database.h>
#include <osquery/devtools/devtools.h>
#include <osquery/filesystem/fileops.h>
#include <osquery/flags.h>
#include <osquery/logger.h>
#include <osquery/main/main.h>
#include <osquery/registry_factory.h>
#include <osquery/sql/sqlite_util.h>
#include <osquery/system.h>

//#include <osquery/experimental/tracing/syscalls_tracing.h>

namespace fs = boost::filesystem;

namespace osquery {

SHELL_FLAG(int32,
           profile,
           0,
           "Enable profile mode when non-0, set number of iterations");

HIDDEN_FLAG(int32,
            profile_delay,
            0,
            "Sleep a number of seconds before and after the profiling");

DECLARE_bool(disable_caching);

int profile(int argc, char* argv[]) {
  std::string query;
  if (!osquery::platformIsatty(stdin)) {
    std::getline(std::cin, query);
  } else if (argc < 2) {
    // No query input provided via stdin or as a positional argument.
    std::cerr << "No query provided via stdin or args to profile..."
              << std::endl;
    return 2;
  } else {
    query = std::string(argv[1]);
  }

  // Perform some duplication from Initializer with respect to database setup.
  osquery::DatabasePlugin::setAllowOpen(true);
  osquery::RegistryFactory::get().setActive("database", "ephemeral");

  auto dbc = osquery::SQLiteDBManager::get();
  for (size_t i = 0; i < static_cast<size_t>(osquery::FLAGS_profile); ++i) {
    osquery::QueryData results;
    auto status = osquery::queryInternal(query, results, dbc);
    dbc->clearAffectedTables();
    if (!status) {
      std::cerr << "Query failed (" << status.getCode()
                << "): " << status.what() << std::endl;
      return status.getCode();
    }
  }

  return 0;
}

int startDaemon(Initializer& runner) {
  runner.start();

//  osquery::events::init_syscall_tracing();

  // Finally wait for a signal / interrupt to shutdown.
  runner.waitForShutdown();
  return 0;
}

int startShell(osquery::Initializer& runner, int argc, char* argv[]) {
  // Check for shell-specific switches and positional arguments.
  if (argc > 1 || !osquery::platformIsatty(stdin) ||
      !osquery::FLAGS_A.empty() || !osquery::FLAGS_pack.empty() ||
      osquery::FLAGS_L || osquery::FLAGS_profile > 0) {
    // A query was set as a positional argument, via stdin, or profiling is on.
    osquery::FLAGS_disable_events = true;
    osquery::FLAGS_disable_caching = true;
  }

  int retcode = 0;
  if (osquery::FLAGS_profile <= 0) {
    runner.start();

    // Virtual tables will be attached to the shell's in-memory SQLite DB.
    retcode = osquery::launchIntoShell(argc, argv);
  } else {
    retcode = profile(argc, argv);
  }
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
