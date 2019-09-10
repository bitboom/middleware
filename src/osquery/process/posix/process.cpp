/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <vector>

#include <osquery/logger.h>
#include <osquery/process/process.h>
#include <osquery/system.h>
#include <osquery/utils/info/platform_type.h>

#ifndef NSIG
#define NSIG 32
#endif

extern char** environ;

namespace osquery {

PlatformProcess::PlatformProcess(PlatformPidType id) : id_(id) {}

bool PlatformProcess::operator==(const PlatformProcess& process) const {
  return (nativeHandle() == process.nativeHandle());
}

bool PlatformProcess::operator!=(const PlatformProcess& process) const {
  return (nativeHandle() != process.nativeHandle());
}

PlatformProcess::~PlatformProcess() {}

int PlatformProcess::pid() const {
  return id_;
}

bool PlatformProcess::kill() const {
  if (!isValid()) {
    return false;
  }

  int status = ::kill(nativeHandle(), SIGKILL);
  return (status == 0);
}

bool PlatformProcess::killGracefully() const {
  if (!isValid()) {
    return false;
  }

  int status = ::kill(nativeHandle(), SIGTERM);
  return (status == 0);
}

ProcessState PlatformProcess::checkStatus(int& status) const {
  int process_status = 0;
  if (!isValid()) {
    return PROCESS_ERROR;
  }

  pid_t result = ::waitpid(nativeHandle(), &process_status, WNOHANG);
  if (result < 0) {
    if (errno == ECHILD) {
      return PROCESS_EXITED;
    }
    process_status = -1;
    return PROCESS_ERROR;
  }

  if (result == 0) {
    return PROCESS_STILL_ALIVE;
  }

  if (WIFEXITED(process_status)) {
    status = WEXITSTATUS(process_status);
    return PROCESS_EXITED;
  }

  // process's state has changed but the state isn't that which we expect!
  return PROCESS_STATE_CHANGE;
}

std::shared_ptr<PlatformProcess> PlatformProcess::getCurrentProcess() {
  pid_t pid = ::getpid();
  return std::make_shared<PlatformProcess>(pid);
}

int PlatformProcess::getCurrentPid() {
  return PlatformProcess::getCurrentProcess()->pid();
}

std::shared_ptr<PlatformProcess> PlatformProcess::getLauncherProcess() {
  pid_t ppid = ::getppid();
  return std::make_shared<PlatformProcess>(ppid);
}

std::shared_ptr<PlatformProcess> PlatformProcess::launchWorker(
    const std::string& exec_path, int argc /* unused */, char** argv) {
  auto worker_pid = ::fork();
  if (worker_pid < 0) {
    return std::shared_ptr<PlatformProcess>();
  } else if (worker_pid == 0) {
    setEnvVar("OSQUERY_WORKER", std::to_string(::getpid()).c_str());
    ::execve(exec_path.c_str(), argv, ::environ);

    // Code should never reach this point
    LOG(ERROR) << "osqueryd could not start worker process";
    ::exit(EXIT_CATASTROPHIC);
    return std::shared_ptr<PlatformProcess>();
  }
  return std::make_shared<PlatformProcess>(worker_pid);
}

std::shared_ptr<PlatformProcess> PlatformProcess::launchTestPythonScript(
    const std::string& args) {
  std::string osquery_path;
  auto osquery_path_option = getEnvVar("OSQUERY_DEPS");
  if (osquery_path_option) {
    osquery_path = *osquery_path_option;
  } else {
    if (!isPlatform(PlatformType::TYPE_FREEBSD)) {
      osquery_path = "/usr/bin/env python";
    } else {
      osquery_path = "/usr/local/bin/python";
    }
  }

  // The whole-string, space-delimited, python process arguments.
  auto argv = osquery_path + " " + args;

  std::shared_ptr<PlatformProcess> process;
  int process_pid = ::fork();
  if (process_pid == 0) {
    // Start a Python script
    ::execlp("sh", "sh", "-c", argv.c_str(), nullptr);
    ::exit(0);
  } else if (process_pid > 0) {
    process.reset(new PlatformProcess(process_pid));
  }

  return process;
}
} // namespace osquery
