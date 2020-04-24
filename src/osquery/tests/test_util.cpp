/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <chrono>
#include <deque>
#include <random>
#include <sstream>
#include <thread>

#include <csignal>
#include <ctime>

#include <gtest/gtest.h>

#include <boost/filesystem/operations.hpp>

#include <osquery/logger.h>
#include <osquery/registry_factory.h>
#include <osquery/sql.h>
#include <osquery/utils/system/time.h>
#include <osquery/utils/conversions/tryto.h>

#include <osquery/tests/test_util.h>

namespace fs = boost::filesystem;

namespace osquery {

/// Will be set with initTesting in test harness main.
std::string kFakeDirectory;

/// Will be set with initTesting in test harness main.
std::string kTestWorkingDirectory;

/// The relative path within the source repo to find test content.
std::string kTestDataPath{"../../../tools/tests/"};

using chrono_clock = std::chrono::high_resolution_clock;

void initTesting() {
  kTestWorkingDirectory =
     (fs::temp_directory_path() / "osquery-tests").string();

  registryAndPluginInit();

  // Allow unit test execution from anywhere in the osquery source/build tree.
  if (fs::exists("test_data/test_inline_pack.conf")) {
    // If the execution occurs within the build/shared directory and shared
    // is pointing to a tmp build directory. See #3414.
    kTestDataPath = "test_data/";
  } else if (fs::exists("../test_data/test_inline_pack.conf")) {
    // ctest executes from the osquery subdirectory. If this is a build/shared
    // link then the test_data directory links to the source repo.
    kTestDataPath = "../test_data/";
  } else {
    while (kTestDataPath.find("tools") != 0) {
      if (!fs::exists(kTestDataPath + "test_inline_pack.conf")) {
        kTestDataPath = kTestDataPath.substr(3, kTestDataPath.size());
      } else {
        break;
      }
    }
  }

  // The tests will fail randomly without test data.
  if (!fs::exists(kTestDataPath)) {
    throw std::runtime_error("Cannot find test data path");
  }

  // Seed the random number generator, some tests generate temporary files
  // ports, sockets, etc using random numbers.
  std::srand(static_cast<unsigned int>(
      chrono_clock::now().time_since_epoch().count()));

  // Set safe default values for path-based flags.
  // Specific unittests may edit flags temporarily.
  kTestWorkingDirectory += std::to_string(1234) + "/";
  kFakeDirectory = kTestWorkingDirectory + kFakeDirectoryName;

  fs::remove_all(kTestWorkingDirectory);
  fs::create_directories(kTestWorkingDirectory);
}

void shutdownTesting() {
}

ScheduledQuery getOsqueryScheduledQuery() {
  ScheduledQuery sq(
      "path_pack",
      "bin",
      "SELECT filename FROM fs WHERE path = '/bin' ORDER BY filename");

  sq.interval = 5;

  return sq;
}

} // namespace osquery
