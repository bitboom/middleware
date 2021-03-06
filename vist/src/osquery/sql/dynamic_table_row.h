/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#pragma once

#include <osquery/core/sql/query_data.h>
#include <osquery/core/sql/table_row.h>

namespace osquery {

/** A TableRow backed by a string map. */
class DynamicTableRow : public TableRow {
public:
	DynamicTableRow() : row() {}
	DynamicTableRow(Row&& r) : row(std::move(r)) {}
	DynamicTableRow(
		std::initializer_list<std::pair<const std::string, std::string>> init)
		: row(init) {}
	DynamicTableRow(const DynamicTableRow&) = delete;
	DynamicTableRow& operator=(const DynamicTableRow&) = delete;
	explicit operator Row() const
	{
		return row;
	}
	virtual int get_rowid(sqlite_int64 default_value, sqlite_int64* pRowid) const;
	virtual int get_column(sqlite3_context* ctx, sqlite3_vtab* pVtab, int col);
	virtual TableRowHolder clone() const;
	inline std::string& operator[](const std::string& key)
	{
		return row[key];
	}
	inline std::string& operator[](std::string&& key)
	{
		return row[key];
	}
	inline size_t count(const std::string& key) const
	{
		return row.count(key);
	}

private:
	Row row;
};
/// Syntactic sugar making DynamicRows inside of TableRowHolders easier to work
/// with. This should go away once strongly typed rows are used everywhere.
class DynamicTableRowHolder {
public:
	DynamicTableRowHolder() : row(new DynamicTableRow()), ptr(row) {}
	DynamicTableRowHolder(
		std::initializer_list<std::pair<const std::string, std::string>> init)
		: row(new DynamicTableRow(init)), ptr(row) {}
	inline operator TableRowHolder&& ()
	{
		return std::move(ptr);
	}
	inline std::string& operator[](const std::string& key)
	{
		return (*row)[key];
	}
	inline std::string& operator[](std::string&& key)
	{
		return (*row)[key];
	}
	inline size_t count(const std::string& key)
	{
		return (*row).count(key);
	}

private:
	DynamicTableRow* row;
	TableRowHolder ptr;
};
inline DynamicTableRowHolder make_table_row()
{
	return DynamicTableRowHolder();
}
inline DynamicTableRowHolder make_table_row(
	std::initializer_list<std::pair<const std::string, std::string>> init)
{
	return DynamicTableRowHolder(init);
}

/// Converts a QueryData struct to TableRows. Intended for use only in
/// generated code.
TableRows tableRowsFromQueryData(QueryData&& rows);

} // namespace osquery
