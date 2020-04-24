/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <chrono>
#include <iostream>
#include <random>
#include <thread>

#include <stdio.h>
#include <time.h>

#include <osquery/system.h>

#ifdef WIN32

#include <osquery/utils/system/system.h>

#include <WbemIdl.h>
#include <signal.h>

#else
#include <unistd.h>
#endif

#ifndef WIN32
#include <sys/resource.h>
#endif

#include <boost/filesystem.hpp>

#include "osquery/utils/config/default_paths.h"
#include "osquery/utils/info/platform_type.h"
#include <osquery/core.h>
#include <osquery/data_logger.h>
#include <osquery/registry.h>
#include <osquery/utils/info/version.h>
#include <osquery/utils/system/time.h>

#ifdef __linux__
#include <sys/syscall.h>

/*
 * These are the io priority groups as implemented by CFQ. RT is the realtime
 * class, it always gets premium service. BE is the best-effort scheduling
 * class, the default for any process. IDLE is the idle scheduling class, it
 * is only served when no one else is using the disk.
 */
enum {
  IOPRIO_CLASS_NONE,
  IOPRIO_CLASS_RT,
  IOPRIO_CLASS_BE,
  IOPRIO_CLASS_IDLE,
};

/*
 * 8 best effort priority levels are supported
 */
#define IOPRIO_BE_NR (8)

enum {
  IOPRIO_WHO_PROCESS = 1,
  IOPRIO_WHO_PGRP,
  IOPRIO_WHO_USER,
};
#endif

#define DESCRIPTION                                                            \
  "osquery %s, your OS as a high-performance relational database\n"
#define EPILOG "\nosquery project page <https://osquery.io>.\n"
#define OPTIONS                                                                \
  "\nosquery configuration options (set by config or CLI flags):\n\n"
#define OPTIONS_SHELL "\nosquery shell-only CLI flags:\n\n"
#define OPTIONS_CLI "osquery%s command line flags:\n\n"
#define USAGE "Usage: %s [OPTION]... %s\n\n"

namespace {
extern "C" {
volatile std::sig_atomic_t kHandledSignal{0};

void signalHandler(int num) {
  // Inform exit status of main threads blocked by service joins.
  if (kHandledSignal == 0) {
    kHandledSignal = num;
    // If no part of osquery requested an interruption then the exit 'wanted'
    // code becomes the signal number.
    if (num != SIGUSR1 && osquery::kExitCode == 0) {
      // The only exception is SIGUSR1 which is used to signal the main thread
      // to interrupt dispatched services.
      osquery::kExitCode = 128 + num;
    }

    if (num == SIGTERM || num == SIGINT || num == SIGABRT ||
        num == SIGUSR1) {
      // Restore the default signal handler.
      std::signal(num, SIG_DFL);
    }
  }
}
}
}

using chrono_clock = std::chrono::high_resolution_clock;

namespace fs = boost::filesystem;

namespace osquery {

ToolType kToolType{ToolType::UNKNOWN};

/// The saved exit code from a thread's request to stop the process.
volatile std::sig_atomic_t kExitCode{0};

/// The saved thread ID for shutdown to short-circuit raising a signal.
static std::thread::id kMainThreadId;

/// Legacy thread ID to ensure that the windows service waits before exiting
unsigned long kLegacyThreadId;

std::function<void()> Initializer::shutdown_{nullptr};
RecursiveMutex Initializer::shutdown_mutex_;

static inline void printUsage(const std::string& binary, ToolType tool) {
  // Parse help options before gflags. Only display osquery-related options.
  fprintf(stdout, DESCRIPTION, kVersion.c_str());
  if (tool == ToolType::SHELL) {
    // The shell allows a caller to run a single SQL statement and exit.
    fprintf(stdout, USAGE, binary.c_str(), "[SQL STATEMENT]");
  } else {
    fprintf(stdout, USAGE, binary.c_str(), "");
  }

  fprintf(stdout, EPILOG);
}

Initializer::Initializer(int& argc,
                         char**& argv,
                         ToolType tool,
                         bool const init_glog)
    : argc_(&argc), argv_(&argv) {
  // Initialize random number generated based on time.
  std::srand(static_cast<unsigned int>(
      chrono_clock::now().time_since_epoch().count()));

  // osquery can function as the daemon or shell depending on argv[0].
  if (tool == ToolType::SHELL_DAEMON) {
    if (fs::path(argv[0]).filename().string().find("osqueryd") !=
        std::string::npos) {
      kToolType = ToolType::DAEMON;
      binary_ = "osqueryd";
    } else {
      kToolType = ToolType::SHELL;
      binary_ = "osqueryi";
    }
  } else {
    // Set the tool type to allow runtime decisions based on daemon, shell, etc.
    kToolType = tool;
  }

  // The 'main' thread is that which executes the initializer.
  kMainThreadId = std::this_thread::get_id();

  // Initialize the COM libs
  platformSetup();
}

void Initializer::initDaemon() const {
}

void Initializer::initShell() const {
}

void Initializer::initWorker(const std::string& name) const {
  // Clear worker's arguments.
  auto original_name = std::string((*argv_)[0]);
  for (int i = 1; i < *argc_; i++) {
    if ((*argv_)[i] != nullptr) {
      memset((*argv_)[i], '\0', strlen((*argv_)[i]));
    }
  }
}

void Initializer::initActivePlugin(const std::string& type,
                                   const std::string& name) const {
  auto rs = RegistryFactory::get().setActive(type, name);
  if (!rs.ok()) {
    LOG(ERROR) << "Cannot activate " << name << " " << type
               << " plugin: " << rs.getMessage();
    requestShutdown(EXIT_CATASTROPHIC);
  }
}

void Initializer::installShutdown(std::function<void()>& handler) {
  RecursiveLock lock(shutdown_mutex_);
  shutdown_ = std::move(handler);
}

void Initializer::start() const {
  // Run the setup for all lazy registries (tables, SQL).
  Registry::setUp();
}

void Initializer::waitForShutdown() {
  {
    RecursiveLock lock(shutdown_mutex_);
    if (shutdown_ != nullptr) {
      // Copy the callable, then remove it, prevent callable recursion.
      auto shutdown = shutdown_;
      shutdown_ = nullptr;

      // Call the shutdown callable.
      shutdown();
    }
  }

  auto excode = (kExitCode != 0) ? kExitCode : EXIT_SUCCESS;
  exit(excode);
}

void Initializer::requestShutdown(int retcode) {
  if (kExitCode == 0) {
    kExitCode = retcode;
  }
}

void Initializer::requestShutdown(int retcode, const std::string& system_log) {
  requestShutdown(retcode);
}

void Initializer::shutdown(int retcode) {
  platformTeardown();
  ::exit(retcode);
}
}
