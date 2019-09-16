/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <algorithm>
#include <ctime>

#include <boost/format.hpp>
#include <boost/io/detail/quoted_manip.hpp>

#include <osquery/config/config.h>
#include <osquery/core.h>
#include <osquery/data_logger.h>
#include <osquery/database.h>
#include <osquery/flags.h>
#include <osquery/numeric_monitoring.h>
#include <osquery/process/process.h>
#include <osquery/query.h>
#include <osquery/utils/system/time.h>

#include "osquery/dispatcher/scheduler.h"
#include "osquery/sql/sqlite_util.h"
#include "osquery/plugins/config/parsers/decorators.h"

namespace osquery {

FLAG(uint64, schedule_timeout, 0, "Limit the schedule, 0 for no limit");

FLAG(uint64,
     schedule_max_drift,
     60,
     "Max time drift in seconds. Scheduler tries to compensate the drift until "
     "the drift exceed this value. After it the drift will be reseted to zero "
     "and the compensation process will start from the beginning. It is needed "
     "to avoid the problem of endless compensation (which is CPU greedy) after "
     "a long SIGSTOP/SIGCONT pause or something similar. Set it to zero to "
     "switch off a drift compensation. Default: 60");

FLAG(uint64,
     schedule_reload,
     300,
     "Interval in seconds to reload database arenas");

FLAG(uint64, schedule_epoch, 0, "Epoch for scheduled queries");

HIDDEN_FLAG(bool,
            schedule_reload_sql,
            false,
            "Reload the SQL implementation during schedule reload");

/// Used to bypass (optimize-out) the set-differential of query results.
DECLARE_bool(events_optimize);
DECLARE_bool(enable_numeric_monitoring);

SQLInternal monitor(const std::string& name, const ScheduledQuery& query) {
  if (FLAGS_enable_numeric_monitoring) {
    return SQLInternal(query.query, true);
  } else {
    // Snapshot the performance and times for the worker before running.
    auto pid = std::to_string(PlatformProcess::getCurrentPid());
    auto r0 = SQL::selectFrom({"resident_size", "user_time", "system_time"},
                              "processes",
                              "pid",
                              EQUALS,
                              pid);
    auto t0 = getUnixTime();
    Config::get().recordQueryStart(name);
    SQLInternal sql(query.query, true);
    // Snapshot the performance after, and compare.
    auto t1 = getUnixTime();
    auto r1 = SQL::selectFrom({"resident_size", "user_time", "system_time"},
                              "processes",
                              "pid",
                              EQUALS,
                              pid);
    if (r0.size() > 0 && r1.size() > 0) {
      // Always called while processes table is working.
      Config::get().recordQueryPerformance(name, t1 - t0, r0[0], r1[0]);
    }
    return sql;
  }
}

Status launchQuery(const std::string& name, const ScheduledQuery& query) {
  // Execute the scheduled query and create a named query object.
  LOG(INFO) << "Executing scheduled query " << name << ": " << query.query;
  runDecorators(DECORATE_ALWAYS);

  auto sql = monitor(name, query);
  if (!sql.getStatus().ok()) {
    LOG(ERROR) << "Error executing scheduled query " << name << ": "
               << sql.getStatus().toString();
    return Status::failure("Error executing scheduled query");
  }

  // Fill in a host identifier fields based on configuration or availability.
  std::string ident = getHostIdentifier();

  // A query log item contains an optional set of differential results or
  // a copy of the most-recent execution alongside some query metadata.
  QueryLogItem item;
  item.name = name;
  item.identifier = ident;
  item.time = osquery::getUnixTime();
  item.epoch = FLAGS_schedule_epoch;
  item.calendar_time = osquery::getAsciiTime();
  getDecorations(item.decorations);

  if (query.options.count("snapshot") && query.options.at("snapshot")) {
    // This is a snapshot query, emit results with a differential or state.
    item.snapshot_results = std::move(sql.rowsTyped());
    logSnapshotQuery(item);
    return Status::success();
  }

  // Create a database-backed set of query results.
  auto dbQuery = Query(name, query);
  // Comparisons and stores must include escaped data.
  sql.escapeResults();
  Status status;
  DiffResults& diff_results = item.results;
  // Add this execution's set of results to the database-tracked named query.
  // We can then ask for a differential from the last time this named query
  // was executed by exact matching each row.
  if (!FLAGS_events_optimize || !sql.eventBased()) {
    status = dbQuery.addNewResults(
        std::move(sql.rowsTyped()), item.epoch, item.counter, diff_results);
    if (!status.ok()) {
      std::string line = "Error adding new results to database for query " +
                         name + ": " + status.what();
      LOG(ERROR) << line;

      // If the database is not available then the daemon cannot continue.
      Initializer::requestShutdown(EXIT_CATASTROPHIC, line);
    }
  } else {
    diff_results.added = std::move(sql.rowsTyped());
  }

  if (query.options.count("removed") && !query.options.at("removed")) {
    diff_results.removed.clear();
  }

  if (diff_results.added.empty() && diff_results.removed.empty()) {
    // No diff results or events to emit.
    return status;
  }

  VLOG(1) << "Found results for query: " << name;

  status = logQueryLogItem(item);
  if (!status.ok()) {
    // If log directory is not available, then the daemon shouldn't continue.
    std::string error = "Error logging the results of query: " + name + ": " +
                        status.toString();
    LOG(ERROR) << error;
    Initializer::requestShutdown(EXIT_CATASTROPHIC, error);
  }
  return status;
}

void SchedulerRunner::start() {
  // Start the counter at the second.
  auto i = osquery::getUnixTime();
  for (; (timeout_ == 0) || (i <= timeout_); ++i) {
    auto start_time_point = std::chrono::steady_clock::now();
    Config::get().scheduledQueries(
        ([&i](const std::string& name, const ScheduledQuery& query) {
          if (query.splayed_interval > 0 && i % query.splayed_interval == 0) {
            TablePlugin::kCacheInterval = query.splayed_interval;
            TablePlugin::kCacheStep = i;
            const auto status = launchQuery(name, query);
            monitoring::record(
                (boost::format("scheduler.query.%s.%s.status.%s") %
                 query.pack_name % query.name %
                 (status.ok() ? "success" : "failure"))
                    .str(),
                1,
                monitoring::PreAggregationType::Sum,
                true);
          }
        }));
    // Configuration decorators run on 60 second intervals only.
    if ((i % 60) == 0) {
      runDecorators(DECORATE_INTERVAL, i);
    }
    if (FLAGS_schedule_reload > 0 && (i % FLAGS_schedule_reload) == 0) {
      if (FLAGS_schedule_reload_sql) {
        SQLiteDBManager::resetPrimary();
      }
      resetDatabase();
    }

    // GLog is not re-entrant, so logs must be flushed in a dedicated thread.
    if ((i % 3) == 0) {
      relayStatusLogs(true);
    }
    auto loop_step_duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time_point);
    if (loop_step_duration + time_drift_ < interval_) {
      pause(std::chrono::milliseconds(interval_ - loop_step_duration -
                                      time_drift_));
      time_drift_ = std::chrono::milliseconds::zero();
    } else {
      time_drift_ += loop_step_duration - interval_;
      if (time_drift_ > max_time_drift_) {
        // giving up
        time_drift_ = std::chrono::milliseconds::zero();
      }
    }
    if (interrupted()) {
      break;
    }
  }
}

std::chrono::milliseconds SchedulerRunner::getCurrentTimeDrift() const
    noexcept {
  return time_drift_;
}

void startScheduler() {
  startScheduler(static_cast<unsigned long int>(FLAGS_schedule_timeout), 1);
}

void startScheduler(unsigned long int timeout, size_t interval) {
  Dispatcher::addService(std::make_shared<SchedulerRunner>(
      timeout, interval, std::chrono::seconds{FLAGS_schedule_max_drift}));
}
} // namespace osquery
