/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include "filesystem_logger.h"

#include <exception>

#include <osquery/logger.h>
#include <osquery/utils/config/default_paths.h>

namespace fs = boost::filesystem;

namespace osquery {

const std::string kFilesystemLoggerFilename = "osqueryd.results.log";
const std::string kFilesystemLoggerSnapshots = "osqueryd.snapshots.log";

Status FilesystemLoggerPlugin::setUp() {
  // Ensure that we create the results log here.
  return logStringToFile("", kFilesystemLoggerFilename, true);
}

Status FilesystemLoggerPlugin::logString(const std::string& s) {
  return logStringToFile(s, kFilesystemLoggerFilename);
}

Status FilesystemLoggerPlugin::logStringToFile(const std::string& s,
                                               const std::string& filename,
                                               bool empty) {
  return Status(1, "Not supported.");
}

Status FilesystemLoggerPlugin::logStatus(
    const std::vector<StatusLogLine>& log) {
  for (const auto& item : log) {
    // Emit this intermediate log to the Glog filesystem logger.
    google::LogMessage(item.filename.c_str(),
                       static_cast<int>(item.line),
                       (google::LogSeverity)item.severity)
            .stream()
        << item.message;
  }

  return Status(0, "OK");
}

Status FilesystemLoggerPlugin::logSnapshot(const std::string& s) {
  // Send the snapshot data to a separate filename.
  return logStringToFile(s, kFilesystemLoggerSnapshots);
}

void FilesystemLoggerPlugin::init(const std::string& name,
                                  const std::vector<StatusLogLine>& log) {
  // Stop the internal Glog facilities.
  google::ShutdownGoogleLogging();


  google::InitGoogleLogging(name.c_str());

  // We may violate Glog global object assumptions. So set names manually.
  auto basename = (log_path_ / name).string();

  google::SetLogDestination(google::GLOG_INFO, (basename + ".INFO.").c_str());
  google::SetLogDestination(google::GLOG_WARNING,
                            (basename + ".WARNING.").c_str());
  google::SetLogDestination(google::GLOG_ERROR, (basename + ".ERROR.").c_str());

  // Now funnel the intermediate status logs provided to `init`.
  logStatus(log);
}
}
