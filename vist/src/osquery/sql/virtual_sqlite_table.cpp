/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <osquery/core.h>
#include <vist/logger.hpp>
#include <osquery/sql.h>

#include "osquery/sql/dynamic_table_row.h"
#include "osquery/sql/sqlite_util.h"

#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;
namespace errc = boost::system::errc;

namespace osquery {

const char* getSystemVFS(bool respect_locking)
{
	return "unix-none";
}

Status genSqliteTableRow(sqlite3_stmt* stmt,
						 TableRows& qd,
						 const fs::path& sqlite_db)
{
	auto r = make_table_row();
	for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
		auto column_name = std::string(sqlite3_column_name(stmt, i));
		auto column_type = sqlite3_column_type(stmt, i);
		switch (column_type) {
		case SQLITE_TEXT: {
			auto text_value = sqlite3_column_text(stmt, i);
			if (text_value != nullptr) {
				r[column_name] = std::string(reinterpret_cast<const char*>(text_value));
			}
			break;
		}
		case SQLITE_FLOAT: {
			auto float_value = sqlite3_column_double(stmt, i);
			r[column_name] = DOUBLE(float_value);
			break;
		}
		case SQLITE_INTEGER: {
			auto int_value = sqlite3_column_int(stmt, i);
			r[column_name] = INTEGER(int_value);
			break;
		}
		}
	}
	if (r.count("path") > 0) {
		WARN(OSQUERY) << "Row contains a path key, refusing to overwrite";
	} else {
		r["path"] = sqlite_db.string();
	}
	qd.push_back(std::move(r));
	return Status::success();
}

Status genTableRowsForSqliteTable(const fs::path& sqlite_db,
								  const std::string& sqlite_query,
								  TableRows& results,
								  bool respect_locking)
{
	sqlite3* db = nullptr;
	boost::system::error_code ec;
	if (sqlite_db.empty()) {
		return Status(1, "Database path does not exist");
	}

	// A tri-state determination of presence
	if (!fs::exists(sqlite_db, ec) || ec.value() != errc::success) {
		return Status(1, ec.message());
	}

	auto rc = sqlite3_open_v2(
				  sqlite_db.string().c_str(),
				  &db,
				  (SQLITE_OPEN_READONLY | SQLITE_OPEN_PRIVATECACHE | SQLITE_OPEN_NOMUTEX),
				  getSystemVFS(respect_locking));
	if (rc != SQLITE_OK || db == nullptr) {
		DEBUG(OSQUERY) << "Cannot open specified database: "
					   << getStringForSQLiteReturnCode(rc);
		if (db != nullptr) {
			sqlite3_close(db);
		}
		return Status(1, "Could not open database");
	}

	sqlite3_stmt* stmt = nullptr;
	rc = sqlite3_prepare_v2(db, sqlite_query.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		sqlite3_close(db);
		DEBUG(OSQUERY) << "Could not prepare database at path: " << sqlite_db;
		return Status(rc, "Could not prepare database");
	}

	while ((sqlite3_step(stmt)) == SQLITE_ROW) {
		auto s = genSqliteTableRow(stmt, results, sqlite_db);
		if (!s.ok()) {
			break;
		}
	}

	// Close handles and free memory
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return Status{};
}
} // namespace osquery
