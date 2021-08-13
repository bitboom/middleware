/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include "dynamic_table_row.h"
#include "virtual_table.h"

#include <vist/logger.hpp>
#include <osquery/utils/conversions/tryto.h>

namespace osquery {

TableRows tableRowsFromQueryData(QueryData&& rows)
{
	TableRows result;

	for (auto&& row : rows) {
		result.push_back(TableRowHolder(new DynamicTableRow(std::move(row))));
	}

	return result;
}

int DynamicTableRow::get_rowid(sqlite_int64 default_value,
							   sqlite_int64* pRowid) const
{
	auto& current_row = this->row;
	auto rowid_it = current_row.find("rowid");
	if (rowid_it != current_row.end()) {
		const auto& rowid_text_field = rowid_it->second;

		auto exp = tryTo<long long>(rowid_text_field, 10);
		if (exp.isError()) {
			DEBUG(OSQUERY) << "Invalid rowid value returned " << exp.getError();
			return SQLITE_ERROR;
		}
		*pRowid = exp.take();

	} else {
		*pRowid = default_value;
	}
	return SQLITE_OK;
}

int DynamicTableRow::get_column(sqlite3_context* ctx,
								sqlite3_vtab* vtab,
								int col)
{
	VirtualTable* pVtab = (VirtualTable*)vtab;
	auto& column_name = std::get<0>(pVtab->content->columns[col]);
	auto& type = std::get<1>(pVtab->content->columns[col]);
	if (pVtab->content->aliases.count(column_name)) {
		// Overwrite the aliased column with the type and name of the new column.
		type = std::get<1>(
				   pVtab->content->columns[pVtab->content->aliases.at(column_name)]);
		column_name = std::get<0>(
						  pVtab->content->columns[pVtab->content->aliases.at(column_name)]);
	}

	// Attempt to cast each xFilter-populated row/column to the SQLite type.
	const auto& value = row[column_name];
	if (this->row.count(column_name) == 0) {
		// Missing content.
		DEBUG(OSQUERY) << "Error " << column_name << " is empty";
		sqlite3_result_null(ctx);
	} else if (type == TEXT_TYPE || type == BLOB_TYPE) {
		sqlite3_result_text(
			ctx, value.c_str(), static_cast<int>(value.size()), SQLITE_STATIC);
	} else if (type == INTEGER_TYPE) {
		auto afinite = tryTo<long>(value, 0);
		if (afinite.isError()) {
			DEBUG(OSQUERY) << "Error casting " << column_name << " (" << value
						   << ") to INTEGER";
			sqlite3_result_null(ctx);
		} else {
			sqlite3_result_int(ctx, afinite.take());
		}
	} else if (type == BIGINT_TYPE || type == UNSIGNED_BIGINT_TYPE) {
		auto afinite = tryTo<long long>(value, 0);
		if (afinite.isError()) {
			DEBUG(OSQUERY) << "Error casting " << column_name << " (" << value
						   << ") to BIGINT";
			sqlite3_result_null(ctx);
		} else {
			sqlite3_result_int64(ctx, afinite.take());
		}
	} else if (type == DOUBLE_TYPE) {
		char* end = nullptr;
		double afinite = strtod(value.c_str(), &end);
		if (end == nullptr || end == value.c_str() || *end != '\0') {
			DEBUG(OSQUERY) << "Error casting " << column_name << " (" << value
						   << ") to DOUBLE";
			sqlite3_result_null(ctx);
		} else {
			sqlite3_result_double(ctx, afinite);
		}
	} else {
		ERROR(OSQUERY) << "Error unknown column type " << column_name;
	}

	return SQLITE_OK;
}

TableRowHolder DynamicTableRow::clone() const
{
	Row new_row = row;
	return TableRowHolder(new DynamicTableRow(std::move(new_row)));
}

} // namespace osquery
