/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <cstring>
#include <functional>
#include <string>

#include <osquery/hashing/hashing.h>

#include <sqlite3.h>

namespace osquery {

static void hashSqliteValue(sqlite3_context* ctx,
                            int argc,
                            sqlite3_value** argv,
                            HashType ht) {
  if (argc == 0) {
    return;
  }

  if (SQLITE_NULL == sqlite3_value_type(argv[0])) {
    sqlite3_result_null(ctx);
    return;
  }

  // Parse and verify the split input parameters.
  const char* input =
      reinterpret_cast<const char*>(sqlite3_value_text(argv[0]));
  if (input == nullptr) {
    sqlite3_result_null(ctx);
    return;
  }

  auto result = hashFromBuffer(ht, input, strlen(input));
  sqlite3_result_text(
      ctx, result.c_str(), static_cast<int>(result.size()), SQLITE_TRANSIENT);
}

static void sqliteMD5Func(sqlite3_context* context,
                          int argc,
                          sqlite3_value** argv) {
  hashSqliteValue(context, argc, argv, HASH_TYPE_MD5);
}

static void sqliteSHA1Func(sqlite3_context* context,
                           int argc,
                           sqlite3_value** argv) {
  hashSqliteValue(context, argc, argv, HASH_TYPE_SHA1);
}

static void sqliteSHA256Func(sqlite3_context* context,
                             int argc,
                             sqlite3_value** argv) {
  hashSqliteValue(context, argc, argv, HASH_TYPE_SHA256);
}

void registerHashingExtensions(sqlite3* db) {
  sqlite3_create_function(db,
                          "md5",
                          1,
                          SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                          nullptr,
                          sqliteMD5Func,
                          nullptr,
                          nullptr);
  sqlite3_create_function(db,
                          "sha1",
                          1,
                          SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                          nullptr,
                          sqliteSHA1Func,
                          nullptr,
                          nullptr);
  sqlite3_create_function(db,
                          "sha256",
                          1,
                          SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                          nullptr,
                          sqliteSHA256Func,
                          nullptr,
                          nullptr);
}
} // namespace osquery
