/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <algorithm>
#include <string>
#include <vector>

#include <vist/logger.hpp>

#include <osquery/query.h>
#include <osquery/utils/status/status.h>

namespace osquery {

uint64_t Query::getPreviousEpoch() const
{
	return 0;
}

uint64_t Query::getQueryCounter(bool new_query) const
{
	return 0;
}

Status Query::getPreviousQueryResults(QueryDataSet& results) const
{
	return Status::success();
}

std::vector<std::string> Query::getStoredQueryNames()
{
	std::vector<std::string> results;
	return results;
}

bool Query::isQueryNameInDatabase() const
{
	auto names = Query::getStoredQueryNames();
	return std::find(names.begin(), names.end(), name_) != names.end();
}

static inline void saveQuery(const std::string& name,
							 const std::string& query)
{
}

bool Query::isNewQuery() const
{
	return true;
}

Status Query::addNewResults(QueryDataTyped qd,
							const uint64_t epoch,
							uint64_t& counter) const
{
	DiffResults dr;
	return addNewResults(std::move(qd), epoch, counter, dr, false);
}

Status Query::addNewResults(QueryDataTyped current_qd,
							const uint64_t current_epoch,
							uint64_t& counter,
							DiffResults& dr,
							bool calculate_diff) const
{
	// The current results are 'fresh' when not calculating a differential.
	bool fresh_results = !calculate_diff;
	bool new_query = false;
	if (!isQueryNameInDatabase()) {
		// This is the first encounter of the scheduled query.
		fresh_results = true;
		INFO(OSQUERY) << "Storing initial results for new scheduled query: " << name_;
		saveQuery(name_, query_);
	} else if (getPreviousEpoch() != current_epoch) {
		fresh_results = true;
		INFO(OSQUERY) << "New Epoch " << current_epoch << " for scheduled query "
					  << name_;
	} else if (isNewQuery()) {
		// This query is 'new' in that the previous results may be invalid.
		new_query = true;
		INFO(OSQUERY) << "Scheduled query has been updated: " + name_;
		saveQuery(name_, query_);
	}

	// Use a 'target' avoid copying the query data when serializing and saving.
	// If a differential is requested and needed the target remains the original
	// query data, otherwise the content is moved to the differential's added set.
	const auto* target_gd = &current_qd;
	bool update_db = true;
	if (!fresh_results && calculate_diff) {
		// Get the rows from the last run of this query name.
		QueryDataSet previous_qd;
		auto status = getPreviousQueryResults(previous_qd);
		if (!status.ok()) {
			return status;
		}

		// Calculate the differential between previous and current query results.
		dr = diff(previous_qd, current_qd);

		update_db = (!dr.added.empty() || !dr.removed.empty());
	} else {
		dr.added = std::move(current_qd);
		target_gd = &dr.added;
	}

	return Status::success();
}

}
