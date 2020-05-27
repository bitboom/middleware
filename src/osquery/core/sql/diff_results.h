/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#pragma once

#include <osquery/core/sql/query_data.h>
#include <osquery/utils/only_movable.h>

namespace osquery {

/**
 * @brief Data structure representing the difference between the results of
 * two queries
 *
 * The representation of two diffed QueryData result sets. Given and old and
 * new QueryData, DiffResults indicates the "added" subset of rows and the
 * "removed" subset of rows.
 */
struct DiffResults : private only_movable {
public:
	/// vector of added rows
	QueryDataTyped added;

	/// vector of removed rows
	QueryDataTyped removed;

	DiffResults() {}
	DiffResults(DiffResults&&) = default;
	DiffResults& operator=(DiffResults&&) = default;

	/// equals operator
	bool operator==(const DiffResults& comp) const
	{
		return (comp.added == added) && (comp.removed == removed);
	}

	/// not equals operator
	bool operator!=(const DiffResults& comp) const
	{
		return !(*this == comp);
	}
};

/**
 * @brief Diff QueryDataSet object and QueryData object
 *        and create a DiffResults object
 *
 * @param old_ the "old" set of results.
 * @param new_ the "new" set of results.
 *
 * @return a DiffResults object which indicates the change from old_ to new_
 *
 * @see DiffResults
 */
DiffResults diff(QueryDataSet& old_, QueryDataTyped& new_);

} // namespace osquery
