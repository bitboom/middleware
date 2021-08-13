/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#pragma once

#include <set>
#include <vector>

#include <osquery/core/sql/row.h>

namespace osquery {

/**
 * @brief The result set returned from a osquery SQL query
 *
 * QueryData is the canonical way to represent the results of SQL queries in
 * osquery. It's just a vector of Rows.
 */
using QueryData = std::vector<Row>;

/**
 * @brief The typed result set returned from a osquery SQL query
 *
 * QueryDataTyped is the canonical way to represent the typed results of SQL
 * queries in osquery. It's just a vector of RowTypeds.
 */
using QueryDataTyped = std::vector<RowTyped>;

/**
 * @brief Set representation result returned from a osquery SQL query
 *
 * QueryDataSet -  It's set of Rows for fast search of a specific row.
 */
using QueryDataSet = std::multiset<RowTyped>;

} // namespace osquery
