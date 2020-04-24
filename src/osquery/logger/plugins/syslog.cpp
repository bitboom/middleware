/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <syslog.h>

#include <osquery/logger.h>
#include <osquery/utils/conversions/split.h>

namespace osquery {

class SyslogLoggerPlugin : public LoggerPlugin {
 public:
  Status logString(const std::string& s);
  Status init(const std::string& name, const std::vector<StatusLogLine>& log);
  Status logStatus(const std::vector<StatusLogLine>& log);
};

REGISTER(SyslogLoggerPlugin, "logger", "syslog");

Status SyslogLoggerPlugin::logString(const std::string& s) {
  for (const auto& line : osquery::split(s, "\n")) {
    syslog(LOG_INFO, "result=%s", line.c_str());
  }
  return Status(0, "OK");
}

Status SyslogLoggerPlugin::logStatus(const std::vector<StatusLogLine>& log) {
  for (const auto& item : log) {
    int severity = LOG_NOTICE;
    if (item.severity == O_INFO) {
      severity = LOG_NOTICE;
    } else if (item.severity == O_WARNING) {
      severity = LOG_WARNING;
    } else if (item.severity == O_ERROR) {
      severity = LOG_ERR;
    } else if (item.severity == O_FATAL) {
      severity = LOG_CRIT;
    }

    std::string line = "severity=" + std::to_string(item.severity)
                    + " location=" + item.filename + ":" + std::to_string(item.line) +
                      " message=" + item.message;

    syslog(severity, "%s", line.c_str());
  }
  return Status(0, "OK");
}

Status SyslogLoggerPlugin::init(const std::string& name,
                                const std::vector<StatusLogLine>& log) {
  closelog();

  // Now funnel the intermediate status logs provided to `init`.
  return logStatus(log);
}
}
