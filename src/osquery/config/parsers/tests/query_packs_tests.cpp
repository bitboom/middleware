/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <gtest/gtest.h>

#include <osquery/logger.h>

#include "osquery/core/test_util.h"

namespace pt = boost::property_tree;

namespace osquery {

// Test the pack version checker.
bool versionChecker(const std::string& pack, const std::string& version);
// Test the pack platform checker.
bool platformChecker(const std::string& required, const std::string& platform);

pt::ptree getQueryPacksContent() {
  pt::ptree pack_tree;
  auto pack_path = kTestDataPath + "test_pack.conf";
  auto status = osquery::parseJSON(pack_path, pack_tree);
  return pack_tree.get_child("queries");
}

std::map<std::string, pt::ptree> getQueryPacksExpectedResults() {
  std::map<std::string, pt::ptree> result;
  pt::ptree aux_data;

  std::string query = "select * from launchd";
  aux_data.put("query", query);
  int interval = 414141;
  aux_data.put("interval", interval);
  std::string platform = "whatever";
  aux_data.put("platform", platform);
  std::string version = "1.0.0";
  aux_data.put("version", version);
  std::string description = "Very descriptive description";
  aux_data.put("description", description);
  std::string value = "Value overflow";
  aux_data.put("value", value);

  result.insert(std::pair<std::string, pt::ptree>("launchd", aux_data));

  return result;
}

class QueryPacksConfigTests : public testing::Test {};

TEST_F(QueryPacksConfigTests, version_comparisons) {
  EXPECT_TRUE(versionChecker("1.0.0", "1.0.0"));
  EXPECT_TRUE(versionChecker("1.0.0", "1.2.0"));
  EXPECT_TRUE(versionChecker("1.0", "1.2.0"));
  EXPECT_TRUE(versionChecker("1.0", "1.0.2"));
  EXPECT_TRUE(versionChecker("1.0.0", "1.0.2-r1"));
  EXPECT_FALSE(versionChecker("1.2", "1.0.2"));
  EXPECT_TRUE(versionChecker("1.0.0-r1", "1.0.0"));
}

TEST_F(QueryPacksConfigTests, platform_comparisons) {
#ifdef __linux__
  // If the platform is linux and the required platform is linux, match
  EXPECT_TRUE(platformChecker("linux", "ubuntu"));
  EXPECT_TRUE(platformChecker("linux", "who_knows_what"));
#endif
  EXPECT_TRUE(platformChecker("linux,darwin", "darwin"));
  EXPECT_TRUE(platformChecker("darwin", "darwin"));
  EXPECT_FALSE(platformChecker("darwin", "linux"));

  EXPECT_TRUE(platformChecker(" darwin", "darwin"));
  // There are no logical operators, just matching.
  EXPECT_TRUE(platformChecker("!darwin", "darwin"));

  EXPECT_TRUE(platformChecker("all", "darwin"));
  EXPECT_TRUE(platformChecker("any", "darwin"));
}

TEST_F(QueryPacksConfigTests, test_query_packs_configuration) {
  auto data = getQueryPacksContent();
  auto expected = getQueryPacksExpectedResults();
  auto& real_ld = data.get_child("launchd");
  auto& expect_ld = expected["launchd"];

  EXPECT_EQ(expect_ld.get("query", ""), real_ld.get("query", ""));
  EXPECT_EQ(expect_ld.get("interval", 0), real_ld.get("interval", 0));
  EXPECT_EQ(expect_ld.get("platform", ""), real_ld.get("platform", ""));
  EXPECT_EQ(expect_ld.get("version", ""), real_ld.get("version", ""));
  EXPECT_EQ(expect_ld.get("description", ""), real_ld.get("description", ""));
  EXPECT_EQ(expect_ld.get("value", ""), real_ld.get("value", ""));
}
}
