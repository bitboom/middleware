// Copyright 2004-present Facebook. All Rights Reserved.

#include <glog/logging.h>

#include "osquery/core.h"
#include "osquery/flags.h"
#include "osquery/filesystem.h"
#include "osquery/registry.h"

namespace osquery {

#define __GFLAGS_NAMESPACE google

const std::string kDescription =
    "your operating system as a high-performance "
    "relational database";
const std::string kEpilog = "osquery project page <http://osquery.io>.";

DEFINE_osquery_flag(string,
                    osquery_log_dir,
                    "/var/log/osquery/",
                    "Directory to store results logging.");

static const char* basename(const char* filename) {
  const char* sep = strrchr(filename, '/');
  return sep ? sep + 1 : filename;
}

void initOsquery(int argc, char* argv[], int tool) {
  std::string binary(basename(argv[0]));
  std::string first_arg = (argc > 1) ? std::string(argv[1]) : "";

  if ((first_arg == "--help" || first_arg == "-h" || first_arg == "-help") &&
      tool != OSQUERY_TOOL_TEST) {
    // Parse help options before gflags. Only display osquery-related options.
    fprintf(stdout, "osquery " OSQUERY_VERSION ", %s\n", kDescription.c_str());
    if (tool == OSQUERY_TOOL_SHELL) {
      // The shell allows a caller to run a single SQL statement and exit.
      fprintf(
          stdout, "Usage: %s [OPTION]... [SQL STATEMENT]\n\n", binary.c_str());
    } else {
      fprintf(stdout, "Usage: %s [OPTION]...\n\n", binary.c_str());
    }
    fprintf(stdout,
            "The following options control the osquery "
            "daemon and shell.\n\n");

    Flag::print_flags(Flag::get().flags());

    if (tool == OSQUERY_TOOL_SHELL) {
      // Print shell flags.
      fprintf(stdout, "\nThe following options control the osquery shell.\n\n");
      Flag::print_flags(Flag::get().shellFlags());
    }

    fprintf(stdout, "\n%s\n", kEpilog.c_str());

    ::exit(0);
  }

  FLAGS_alsologtostderr = true;
  FLAGS_logbufsecs = 0; // flush the log buffer immediately
  FLAGS_stop_logging_if_full_disk = true;
  FLAGS_max_log_size = 1024; // max size for individual log file is 1GB

  // Set version string from CMake build
  __GFLAGS_NAMESPACE::SetVersionString(OSQUERY_VERSION);

  // Let gflags parse the non-help options/flags.
  __GFLAGS_NAMESPACE::ParseCommandLineFlags(&argc, &argv, false);

  if (isWritable(FLAGS_osquery_log_dir.c_str()).ok()) {
    FLAGS_log_dir = FLAGS_osquery_log_dir;
  }

  google::InitGoogleLogging(argv[0]);
  osquery::InitRegistry::get().run();
}
}
