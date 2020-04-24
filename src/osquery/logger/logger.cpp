/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#ifndef WIN32
#include <syslog.h>
#endif

#include <algorithm>
#include <future>
#include <queue>
#include <thread>

#include <boost/noncopyable.hpp>

#include <osquery/data_logger.h>
#include <osquery/plugins/logger.h>
#include <osquery/registry_factory.h>
#include <osquery/system.h>

#include <osquery/utils/conversions/split.h>
#include <osquery/utils/info/platform_type.h>
#include <osquery/utils/json/json.h>
#include <osquery/utils/system/time.h>

namespace rj = rapidjson;

namespace osquery {

CREATE_REGISTRY(LoggerPlugin, "logger");

/**
 * @brief A custom Glog log sink for forwarding or buffering status logs.
 *
 * This log sink has two modes, it can buffer Glog status logs until an osquery
 * logger is initialized or forward Glog status logs to an initialized and
 * appropriate logger. The appropriateness is determined by the logger when its
 * LoggerPlugin::init method is called. If the `init` method returns success
 * then a BufferedLogSink will start forwarding status logs to
 * LoggerPlugin::logStatus.
 *
 * This facility will start buffering when first used and stop buffering
 * (aka remove itself as a Glog sink) using the exposed APIs. It will live
 * throughout the life of the process for two reasons: (1) It makes sense when
 * the active logger plugin is handling Glog status logs and (2) it must remove
 * itself as a Glog target.
 */
class BufferedLogSink : public google::LogSink, private boost::noncopyable {
 public:
  /// We create this as a Singleton for proper disable/shutdown.
  static BufferedLogSink& get();

  /// The Glog-API LogSink call-in method.
  void send(google::LogSeverity severity,
            const char* full_filename,
            const char* base_filename,
            int line,
            const struct ::tm* tm_time,
            const char* message,
            size_t message_len) override;

  /// Pop from the aync sender queue and wait for one send to complete.
  void WaitTillSent() override;

 public:
  /// Accessor/mutator to dump all of the buffered logs.
  std::vector<StatusLogLine>& dump();

  /// Add the buffered log sink to Glog.
  void enable();

  /// Start the Buffered Sink, without enabling forwarding to loggers.
  void setUp();

  /**
   * @brief Add a logger plugin that should receive status updates.
   *
   * Since the logger may support multiple active logger plugins the sink
   * will keep track of those plugins that returned success after ::init.
   * This list of plugins will received forwarded messages from the sink.
   *
   * This list is important because sending logs to plugins that also use
   * and active Glog Sink (supports multiple) will create a logging loop.
   */
  void addPlugin(const std::string& name);

  /// Clear the sinks list, clear the named plugins added by addPlugin.s
  void resetPlugins();

  /// Retrieve the list of enabled plugins that should have logs forwarded.
  const std::vector<std::string>& enabledPlugins() const;

 public:
  /// Queue of sender functions that relay status logs to all plugins.
  std::queue<std::future<void>> senders;

 public:
  BufferedLogSink(BufferedLogSink const&) = delete;
  void operator=(BufferedLogSink const&) = delete;

 private:
  /// Create the log sink as buffering or forwarding.
  BufferedLogSink() = default;

  /// Stop the log sink.
  ~BufferedLogSink();

 private:
  /// Intermediate log storage until an osquery logger is initialized.
  std::vector<StatusLogLine> logs_;

  /**
   * @Brief Is the logger temporarily disabled.
   *
   * The Google Log Sink will still be active, but the send method also checks
   * enabled and drops log lines to the flood if the forwarder is not enabled.
   */
  std::atomic<bool> enabled_{false};

  /// Track multiple loggers that should receive sinks from the send forwarder.
  std::vector<std::string> sinks_;
};

/// Mutex protecting accesses to buffered status logs.
Mutex kBufferedLogSinkLogs;

/// Mutex protecting queued status log futures.
Mutex kBufferedLogSinkSenders;

static void serializeIntermediateLog(const std::vector<StatusLogLine>& log,
                                     PluginRequest& request) {
  auto doc = JSON::newArray();
  for (const auto& i : log) {
    auto line = doc.getObject();
    doc.add("s", static_cast<int>(i.severity), line);
    doc.addRef("f", i.filename, line);
    doc.add("i", i.line, line);
    doc.addRef("m", i.message, line);
    doc.addRef("h", i.identifier, line);
    doc.addRef("c", i.calendar_time, line);
    doc.add("u", i.time, line);
    doc.push(line);
  }

  doc.toString(request["log"]);
}

void initStatusLogger(const std::string& name, bool init_glog) {
  // Start the logging, and announce the daemon is starting.
  if (init_glog) {
    google::InitGoogleLogging(name.c_str());
  }
}

void initLogger(const std::string& name) {
  BufferedLogSink::get().resetPlugins();

  bool forward = false;
  PluginRequest init_request = {{"init", name}};
  PluginRequest features_request = {{"action", "features"}};
  auto logger_plugin = RegistryFactory::get().getActive("logger");
  // Allow multiple loggers, make sure each is accessible.
  for (const auto& logger : osquery::split(logger_plugin, ",")) {
    if (!RegistryFactory::get().exists("logger", logger)) {
      continue;
    }

    Registry::call("logger", logger, init_request);
    auto status = Registry::call("logger", logger, features_request);
    if ((status.getCode() & LOGGER_FEATURE_LOGSTATUS) > 0) {
      // Glog status logs are forwarded to logStatus.
      forward = true;
      // To support multiple plugins we only add the names of plugins that
      // return a success status after initialization.
      BufferedLogSink::get().addPlugin(logger);
    }
  }

  if (forward) {
    // Begin forwarding after all plugins have been set up.
    BufferedLogSink::get().enable();
    relayStatusLogs(true);
  }
}

BufferedLogSink& BufferedLogSink::get() {
  static BufferedLogSink sink;
  return sink;
}

void BufferedLogSink::setUp() {
  google::AddLogSink(&get());
}

void BufferedLogSink::enable() {
  enabled_ = true;
}

void BufferedLogSink::send(google::LogSeverity severity,
                           const char* full_filename,
                           const char* base_filename,
                           int line,
                           const struct ::tm* tm_time,
                           const char* message,
                           size_t message_len) {
  // WARNING, be extremely careful when accessing data here.
  // This should not cause any persistent storage or logging actions.
  {
    WriteLock lock(kBufferedLogSinkLogs);
    logs_.push_back({(StatusLogSeverity)severity,
                     std::string(base_filename),
                     static_cast<size_t>(line),
                     std::string(message, message_len),
                     toAsciiTimeUTC(tm_time),
                     toUnixTime(tm_time),
                     std::string()});
  }
}

void BufferedLogSink::WaitTillSent() {
  std::future<void> first;

  {
    WriteLock lock(kBufferedLogSinkSenders);
    if (senders.empty()) {
      return;
    }
    first = std::move(senders.back());
    senders.pop();
  }

  if (!isPlatform(PlatformType::TYPE_WINDOWS)) {
    first.wait();
  } else {
    // Windows is locking by scheduling an async on the main thread.
    first.wait_for(std::chrono::microseconds(100));
  }
}

std::vector<StatusLogLine>& BufferedLogSink::dump() {
  return logs_;
}

void BufferedLogSink::addPlugin(const std::string& name) {
  sinks_.push_back(name);
}

void BufferedLogSink::resetPlugins() {
  sinks_.clear();
}

const std::vector<std::string>& BufferedLogSink::enabledPlugins() const {
  return sinks_;
}

BufferedLogSink::~BufferedLogSink() {
  enabled_ = false;
}

Status logString(const std::string& message, const std::string& category) {
  return logString(
      message, category, RegistryFactory::get().getActive("logger"));
}

Status logString(const std::string& message,
                 const std::string& category,
                 const std::string& receiver) {
  Status status;
  for (const auto& logger : osquery::split(receiver, ",")) {
    if (Registry::get().exists("logger", logger, true)) {
      auto plugin = Registry::get().plugin("logger", logger);
      auto logger_plugin = std::dynamic_pointer_cast<LoggerPlugin>(plugin);
      status = logger_plugin->logString(message);
    } else {
      status = Registry::call(
          "logger", logger, {{"string", message}, {"category", category}});
    }
  }
  return status;
}

namespace {
const std::string kTotalQueryCounterMonitorPath("query.total.count");
}

Status logQueryLogItem(const QueryLogItem& results) {
  return logQueryLogItem(results, RegistryFactory::get().getActive("logger"));
}

Status logQueryLogItem(const QueryLogItem& results,
                       const std::string& receiver) {
  std::vector<std::string> json_items;
  Status status;
  std::string json;
  status = serializeQueryLogItemJSON(results, json);
  json_items.emplace_back(json);

  if (!status.ok()) {
    return status;
  }

  for (const auto& json : json_items) {
    status = logString(json, "event", receiver);
  }
  return status;
}

Status logSnapshotQuery(const QueryLogItem& item) {
  std::vector<std::string> json_items;
  Status status;
  std::string json;
  status = serializeQueryLogItemJSON(item, json);
  json_items.emplace_back(json);

  if (!status.ok()) {
    return status;
  }

  for (const auto& json : json_items) {
    auto receiver = RegistryFactory::get().getActive("logger");
    for (const auto& logger : osquery::split(receiver, ",")) {
      if (Registry::get().exists("logger", logger, true)) {
        auto plugin = Registry::get().plugin("logger", logger);
        auto logger_plugin = std::dynamic_pointer_cast<LoggerPlugin>(plugin);
        status = logger_plugin->logSnapshot(json);
      } else {
        status = Registry::call("logger", logger, {{"snapshot", json}});
      }
    }
  }

  return status;
}

size_t queuedStatuses() {
  ReadLock lock(kBufferedLogSinkLogs);
  return BufferedLogSink::get().dump().size();
}

size_t queuedSenders() {
  ReadLock lock(kBufferedLogSinkSenders);
  return BufferedLogSink::get().senders.size();
}

void relayStatusLogs(bool async) {
  {
    ReadLock lock(kBufferedLogSinkLogs);
    if (BufferedLogSink::get().dump().size() == 0) {
      return;
    }
  }

  auto sender = ([]() {
    auto identifier = getHostIdentifier();

    // Construct a status log plugin request.
    PluginRequest request = {{"status", "true"}};
    {
      WriteLock lock(kBufferedLogSinkLogs);
      auto& status_logs = BufferedLogSink::get().dump();
      for (auto& log : status_logs) {
        // Copy the host identifier into each status log.
        log.identifier = identifier;
      }

      serializeIntermediateLog(status_logs, request);

      // Flush the buffered status logs.
      status_logs.clear();
    }

    auto logger_plugin = RegistryFactory::get().getActive("logger");
    for (const auto& logger : osquery::split(logger_plugin, ",")) {
      auto& enabled = BufferedLogSink::get().enabledPlugins();
      if (std::find(enabled.begin(), enabled.end(), logger) != enabled.end()) {
        // Skip the registry's logic, and send directly to the core's logger.
        PluginResponse response;
        Registry::call("logger", logger, request, response);
      }
    }
  });

  if (async) {
    sender();
  } else {
    std::packaged_task<void()> task(std::move(sender));
    auto result = task.get_future();
    std::thread(std::move(task)).detach();

    // Lock accesses to the sender queue.
    WriteLock lock(kBufferedLogSinkSenders);
    BufferedLogSink::get().senders.push(std::move(result));
  }
}

void systemLog(const std::string& line) {
#ifndef WIN32
  syslog(LOG_NOTICE, "%s", line.c_str());
#endif
}
}
