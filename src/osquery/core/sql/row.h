/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include <boost/variant.hpp>

namespace osquery {

/**
 * @brief Alias for string.
 */
using RowData = std::string;

/**
 * @brief A variant type for the SQLite type affinities.
 */
using RowDataTyped = boost::variant<long long, double, std::string>;

/**
 * @brief A single row from a database query
 *
 * Row is a simple map where individual column names are keys, which map to
 * the Row's respective value
 */
using Row = std::map<std::string, RowData>;

/**
 * @brief A single typed row from a database query
 *
 * RowTyped is a simple map where individual column names are keys, which map to
 * the Row's respective type-variant value
 */
using RowTyped = std::map<std::string, RowDataTyped>;

/**
 * @brief A vector of column names associated with a query
 *
 * ColumnNames is a vector of the column names, in order, returned by a query.
 */
using ColumnNames = std::vector<std::string>;

} // namespace osquery
