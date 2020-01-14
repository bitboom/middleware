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
#include <osquery/filesystem/filesystem.h>
#include <osquery/flags.h>
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

namespace osquery {
CLI_FLAG(uint64, alarm_timeout, 4, "Seconds to wait for a graceful shutdown");
CLI_FLAG(bool,
         enable_signal_handler,
         false,
         "Enable custom osquery signals handler instead of default one");
}

namespace {
extern "C" {
static inline bool hasWorkerVariable() {
  return ::osquery::getEnvVar("OSQUERY_WORKER").is_initialized();
}

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
#ifndef WIN32
      // Time to stop, set an upper bound time constraint on how long threads
      // have to terminate (join). Publishers may be in 20ms or similar sleeps.
      alarm(osquery::FLAGS_alarm_timeout);

      // Allow the OS to auto-reap our child processes.
      std::signal(SIGCHLD, SIG_IGN);
#endif

      // Restore the default signal handler.
      std::signal(num, SIG_DFL);
    }
  }

#ifndef WIN32
  if (num == SIGALRM) {
    // Restore the default signal handler for SIGALRM.
    std::signal(SIGALRM, SIG_DFL);

    // Took too long to stop.
    VLOG(1) << "Cannot stop event publisher threads or services";
    raise((kHandledSignal != 0) ? kHandledSignal : SIGALRM);
  }
#endif

}
}
}

using chrono_clock = std::chrono::high_resolution_clock;

namespace fs = boost::filesystem;

DECLARE_string(flagfile);

namespace osquery {

DECLARE_string(logger_plugin);
DECLARE_bool(config_check);
DECLARE_bool(config_dump);
DECLARE_bool(disable_logging);

CLI_FLAG(bool, S, false, "Run as a shell process");
CLI_FLAG(bool, D, false, "Run as a daemon process");
CLI_FLAG(bool, daemonize, false, "Attempt to daemonize (POSIX only)");

ToolType kToolType{ToolType::UNKNOWN};

/// The saved exit code from a thread's request to stop the process.
volatile std::sig_atomic_t kExitCode{0};

/// The saved thread ID for shutdown to short-circuit raising a signal.
static std::thread::id kMainThreadId;

/// Legacy thread ID to ensure that the windows service waits before exiting
unsigned long kLegacyThreadId;

/// When no flagfile is provided via CLI, attempt to read flag 'defaults'.
const std::string kBackupDefaultFlagfile{OSQUERY_HOME "osquery.flags.default"};

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

  fprintf(stdout, OPTIONS_CLI, "");
  Flag::printFlags(false, false, true);
  fprintf(stdout, OPTIONS);
  Flag::printFlags();

  if (tool == ToolType::SHELL) {
    // Print shell flags.
    fprintf(stdout, OPTIONS_SHELL);
    Flag::printFlags(true);
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

#ifndef WIN32
  // Set the max number of open files.
  struct rlimit nofiles;
  if (getrlimit(RLIMIT_NOFILE, &nofiles) == 0) {
    if (nofiles.rlim_cur < 1024 || nofiles.rlim_max < 1024) {
      nofiles.rlim_cur = (nofiles.rlim_cur < 1024) ? 1024 : nofiles.rlim_cur;
      nofiles.rlim_max = (nofiles.rlim_max < 1024) ? 1024 : nofiles.rlim_max;
      setrlimit(RLIMIT_NOFILE, &nofiles);
    }
  }
#endif

  Flag::create("logtostderr",
               {"Log messages to stderr in addition to the logger plugin(s)",
                false,
                false,
                true,
                false});
  Flag::create("stderrthreshold",
               {"Stderr log level threshold", false, false, true, false});

  // osquery implements a custom help/usage output.
  bool default_flags = true;
  for (int i = 1; i < *argc_; i++) {
    auto help = std::string((*argv_)[i]);
    if (help == "-S" || help == "--S") {
      kToolType = ToolType::SHELL;
      binary_ = "osqueryi";
    } else if (help == "-D" || help == "--D") {
      kToolType = ToolType::DAEMON;
      binary_ = "osqueryd";
    } else if ((help == "--help" || help == "-help" || help == "--h" ||
                help == "-h") &&
               tool != ToolType::TEST) {
      printUsage(binary_, kToolType);
      shutdown();
    }
    if (help.find("--flagfile") == 0) {
      default_flags = false;
    }
  }

  if (isShell()) {
    // The shell is transient, rewrite config-loaded paths.
    FLAGS_disable_logging = true;
    // The shell never will not fork a worker.
  }

  if (default_flags && isReadable(kBackupDefaultFlagfile)) {
    // No flagfile was set (daemons and services always set a flagfile).
    FLAGS_flagfile = kBackupDefaultFlagfile;
  } else {
    // No flagfile was set, but no default flags exist.
    default_flags = false;
  }

  // Set version string from CMake build
  GFLAGS_NAMESPACE::SetVersionString(kVersion.c_str());

  // Let gflags parse the non-help options/flags.
  GFLAGS_NAMESPACE::ParseCommandLineFlags(argc_, argv_, isShell());

  // Initialize registries and plugins
  registryAndPluginInit();

  if (isShell()) {
    // Initialize the shell after setting modified defaults and parsing flags.
    initShell();
  }

  if (FLAGS_enable_signal_handler) {
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGUSR1, signalHandler);

    // All tools handle the same set of signals.
    // If a daemon process is a watchdog the signal is passed to the worker,
    // unless the worker has not yet started.
    if (!isPlatform(PlatformType::TYPE_WINDOWS)) {
      std::signal(SIGTERM, signalHandler);
      std::signal(SIGINT, signalHandler);
      std::signal(SIGHUP, signalHandler);
      std::signal(SIGALRM, signalHandler);
    }
  }

  // Initialize the COM libs
  platformSetup();
}

void Initializer::initDaemon() const {
  if (isWorker() || !isDaemon()) {
    // The worker process (child) will not daemonize.
    return;
  }

#if !defined(__APPLE__) && !defined(WIN32)
  // OS X uses launchd to daemonize.
  if (osquery::FLAGS_daemonize) {
    if (daemon(0, 0) == -1) {
      shutdown(EXIT_FAILURE);
    }
  }
#endif

  // Print the version to the OS system log.
  systemLog(binary_ + " started [version=" + kVersion + "]");
}

void Initializer::initShell() const {
  // Get the caller's home dir for temporary storage/state management.
  auto homedir = osqueryHomeDirectory();
  if (Flag::isDefault("hash_delay")) {
    // The hash_delay is designed for daemons only.
    Flag::updateValue("hash_delay", "0");
  }
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

bool Initializer::isWorker() {
  return hasWorkerVariable();
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

  // Initialize the status and result plugin logger.
  if (!FLAGS_disable_logging) {
    initActivePlugin("logger", FLAGS_logger_plugin);
    initLogger(binary_);
  }
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

  // Hopefully release memory used by global string constructors in gflags.
  GFLAGS_NAMESPACE::ShutDownCommandLineFlags();

  auto excode = (kExitCode != 0) ? kExitCode : EXIT_SUCCESS;
  exit(excode);
}

void Initializer::requestShutdown(int retcode) {
  if (kExitCode == 0) {
    kExitCode = retcode;
  }

  // Stop thrift services/clients/and their thread pools.
  if (std::this_thread::get_id() != kMainThreadId &&
      FLAGS_enable_signal_handler) {
    raise(SIGUSR1);
  } else {
    // The main thread is requesting a shutdown, meaning in almost every case
    // it is NOT waiting for a shutdown.
    // Exceptions include: tight request / wait in an exception handler or
    // custom signal handling.
    waitForShutdown();
  }
}

void Initializer::requestShutdown(int retcode, const std::string& system_log) {
  systemLog(system_log);
  requestShutdown(retcode);
}

void Initializer::shutdown(int retcode) {
  platformTeardown();
  ::exit(retcode);
}
}
