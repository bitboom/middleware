/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <exception>
#include <mutex>

#include <osquery/filesystem.h>
#include <osquery/logger.h>

namespace pt = boost::property_tree;
namespace fs = boost::filesystem;

namespace osquery {

const std::string kFilesystemLoggerFilename = "osqueryd.results.log";
const std::string kFilesystemLoggerSnapshots = "osqueryd.snapshots.log";
const std::string kFilesystemLoggerHealth = "osqueryd.health.log";

std::mutex filesystemLoggerPluginMutex;

class FilesystemLoggerPlugin : public LoggerPlugin {
 public:
  Status setUp();
  Status logString(const std::string& s);
  Status logStringToFile(const std::string& s, const std::string& filename);
  Status logSnapshot(const std::string& s);
  Status logHealth(const std::string& s);
  Status init(const std::string& name, const std::vector<StatusLogLine>& log);
  Status logStatus(const std::vector<StatusLogLine>& log);

 private:
  fs::path log_path_;
};

REGISTER(FilesystemLoggerPlugin, "logger", "filesystem");

Status FilesystemLoggerPlugin::setUp() {
  return Status(0, "OK");
}

Status FilesystemLoggerPlugin::logString(const std::string& s) {
  return logStringToFile(s, kFilesystemLoggerFilename);
}

Status FilesystemLoggerPlugin::logStringToFile(const std::string& s,
                                               const std::string& filename) {
  std::lock_guard<std::mutex> lock(filesystemLoggerPluginMutex);
  try {
    // The results log may contain sensitive information if run as root.
    auto status = writeTextFile((log_path_ / filename).string(), s, 0640, true);
    if (!status.ok()) {
      return status;
    }
  } catch (const std::exception& e) {
    return Status(1, e.what());
  }
  return Status(0, "OK");
}

Status FilesystemLoggerPlugin::logStatus(
    const std::vector<StatusLogLine>& log) {
  for (const auto& item : log) {
    // Emit this intermediate log to the Glog filesystem logger.
    google::LogMessage(item.filename.c_str(),
                       item.line,
                       (google::LogSeverity)item.severity).stream()
        << item.message;
  }

  return Status(0, "OK");
}

Status FilesystemLoggerPlugin::logSnapshot(const std::string& s) {
  // Send the snapshot data to a separate filename.
  return logStringToFile(s, kFilesystemLoggerSnapshots);
}

Status FilesystemLoggerPlugin::logHealth(const std::string& s) {
  return logStringToFile(s, kFilesystemLoggerHealth);
}

Status FilesystemLoggerPlugin::init(const std::string& name,
                                    const std::vector<StatusLogLine>& log) {
  // Stop the internal Glog facilities.
  google::ShutdownGoogleLogging();

  // Restart the Glog facilities using the name `init` was provided.
  google::InitGoogleLogging(name.c_str());

  // We may violate Glog global object assumptions. So set names manually.
  auto basename = (log_path_ / name).string();
  google::SetLogDestination(google::INFO, (basename + ".INFO.").c_str());
  google::SetLogDestination(google::WARNING, (basename + ".WARNING.").c_str());
  google::SetLogDestination(google::ERROR, (basename + ".ERROR.").c_str());

  // Now funnel the intermediate status logs provided to `init`.
  logStatus(log);

  // The filesystem logger cheats and uses Glog to log to the filesystem so
  // we can return failure here and stop the custom log sink.
  return Status(1, "No status logger used for filesystem");
}
}
