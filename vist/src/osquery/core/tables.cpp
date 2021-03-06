/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <osquery/registry_factory.h>
#include <osquery/tables.h>
#include <osquery/utils/conversions/tryto.h>

#include <vist/logger.hpp>

namespace osquery {

CREATE_LAZY_REGISTRY(TablePlugin, "table");

size_t TablePlugin::kCacheInterval = 0;
size_t TablePlugin::kCacheStep = 0;

Status TablePlugin::addExternal(const std::string& name,
								const PluginResponse& response)
{
	// Attach the table.
	if (response.size() == 0) {
		// Invalid table route info.
		// Tables must broadcast their column information, this is used while the
		// core is deciding if the extension's route is valid.
		return Status(1, "Invalid route info");
	}

	// Use the SQL registry to attach the name/definition.
	return Registry::call("sql", "sql", {{"action", "attach"}, {"table", name}});
}

void TablePlugin::removeExternal(const std::string& name)
{
	// Detach the table name.
	Registry::call("sql", "sql", {{"action", "detach"}, {"table", name}});
}

void TablePlugin::setRequestFromContext(const QueryContext& context,
										PluginRequest& request)
{
	vist::json::Json document;
	vist::json::Array constraints;

	// The QueryContext contains a constraint map from column to type information
	// and the list of operand/expression constraints applied to that column from
	// the query given.
	//
	//  {"constraints":[{"name":"test_int","list":[{"op":2,"expr":"2"}],"affinity":"TEXT"}]}
	for (const auto& constraint : context.constraints) {
		auto child = constraint.second.serialize();
		child["name"] = constraint.first;

		constraints.push(child);
	}

	document.push("constraints", constraints);

	if (context.colsUsed) {
		vist::json::Array colsUsed;
		for (const auto& columnName : *context.colsUsed)
			colsUsed.push(columnName);

		document.push("colsUsed", colsUsed);
	}

	request["context"] = document.serialize();
	DEBUG(OSQUERY) << "request context->" << request["context"];
}

QueryContext TablePlugin::getContextFromRequest(const PluginRequest& request) const
{
	QueryContext context;
	if (request.count("context") == 0)
		return context;

	using namespace vist::json;
	std::string serialized = request.at("context");
	Json document = Json::Parse(serialized);
	DEBUG(OSQUERY) << "request context->" << document.serialize();

	if (document.exist("colsUsed")) {
		UsedColumns colsUsed;
		Array array = document.get<Array>("colsUsed");
		for (auto i = 0; i < array.size(); i++) {
			std::string name = array.at(i);
			colsUsed.insert(name);
		}
		context.colsUsed = colsUsed;
	}

	Array constraints = document.get<Array>("constraints");
	for (auto i = 0; i < constraints.size(); i++) {
		auto constraint = Object::Create(constraints.at(i));
		std::string name = constraint["name"];
		context.constraints[name].deserialize(constraint);
	}

	return context;
}

UsedColumnsBitset TablePlugin::usedColumnsToBitset(
	const UsedColumns usedColumns) const
{
	UsedColumnsBitset result;

	const auto columns = this->columns();
	const auto aliases = this->aliasedColumns();
	for (size_t i = 0; i < columns.size(); i++) {
		auto column_name = std::get<0>(columns[i]);
		const auto& aliased_name = aliases.find(column_name);
		if (aliased_name != aliases.end()) {
			column_name = aliased_name->second;
		}
		if (usedColumns.find(column_name) != usedColumns.end()) {
			result.set(i);
		}
	}

	return result;
}

Status TablePlugin::call(const PluginRequest& request,
						 PluginResponse& response)
{
	response.clear();

	// TablePlugin API calling requires an action.
	if (request.count("action") == 0) {
		return Status(1, "Table plugins must include a request action");
	}

	const auto& action = request.at("action");

	if (action == "select") {
		auto context = getContextFromRequest(request);
		response = select(context);
	} else if (action == "delete") {
		auto context = getContextFromRequest(request);
		response = delete_(context, request);
	} else if (action == "insert") {
		auto context = getContextFromRequest(request);
		response = insert(context, request);
	} else if (action == "update") {
		auto context = getContextFromRequest(request);
		response = update(context, request);
	} else if (action == "columns") {
		response = routeInfo();
	} else {
		return Status(1, "Unknown table plugin action: " + action);
	}

	return Status::success();
}

std::string TablePlugin::columnDefinition(bool is_extension) const
{
	return osquery::columnDefinition(columns(), is_extension);
}

PluginResponse TablePlugin::routeInfo() const
{
	// Route info consists of the serialized column information.
	PluginResponse response;
	for (const auto& column : columns()) {
		response.push_back({
			{"id", "column"},
			{"name", std::get<0>(column)},
			{"type", columnTypeName(std::get<1>(column))},
			{"op", INTEGER(static_cast<size_t>(std::get<2>(column)))}});
	}
	// Each table name alias is provided such that the core may add the views.
	// These views need to be removed when the backing table is detached.
	for (const auto& alias : aliases()) {
		response.push_back({{"id", "alias"}, {"alias", alias}});
	}

	// Each column alias must be provided, additionally to the column's option.
	// This sets up the value-replacement move within the SQL implementation.
	for (const auto& target : columnAliases()) {
		for (const auto& alias : target.second) {
			response.push_back(
			{{"id", "columnAlias"}, {"name", alias}, {"target", target.first}});
		}
	}

	response.push_back({
		{"id", "attributes"},
		{"attributes", INTEGER(static_cast<size_t>(attributes()))}});
	return response;
}

static bool cacheAllowed(const TableColumns& cols, const QueryContext& ctx)
{
	if (!ctx.useCache()) {
		// The query execution did not request use of the warm cache.
		return false;
	}

	auto uncachable = ColumnOptions::INDEX | ColumnOptions::REQUIRED |
					  ColumnOptions::ADDITIONAL | ColumnOptions::OPTIMIZED;
	for (const auto& column : cols) {
		auto opts = std::get<2>(column) & uncachable;
		if (opts && ctx.constraints.at(std::get<0>(column)).exists()) {
			return false;
		}
	}
	return true;
}

bool TablePlugin::isCached(size_t step, const QueryContext& ctx) const
{
	// Perform the step comparison first, because it's easy.
	return (step < last_cached_ + last_interval_ && cacheAllowed(columns(), ctx));
}

TableRows TablePlugin::getCache() const
{
	TableRows results;
	return results;
}

void TablePlugin::setCache(size_t step,
						   size_t interval,
						   const QueryContext& ctx,
						   const TableRows& results)
{
	return;
}

std::string columnDefinition(const TableColumns& columns, bool is_extension)
{
	std::map<std::string, bool> epilog;
	bool indexed = false;
	std::vector<std::string> pkeys;

	std::string statement = "(";
	for (size_t i = 0; i < columns.size(); ++i) {
		const auto& column = columns.at(i);
		statement +=
			'`' + std::get<0>(column) + "` " + columnTypeName(std::get<1>(column));
		auto& options = std::get<2>(column);
		if (options & (ColumnOptions::INDEX | ColumnOptions::ADDITIONAL)) {
			if (options & ColumnOptions::INDEX) {
				indexed = true;
			}
			pkeys.push_back(std::get<0>(column));
			epilog["WITHOUT ROWID"] = true;
		}
		if (options & ColumnOptions::HIDDEN) {
			statement += " HIDDEN";
		}
		if (i < columns.size() - 1) {
			statement += ", ";
		}
	}

	// If there are only 'additional' columns (rare), do not attempt a pkey.
	if (!indexed) {
		epilog["WITHOUT ROWID"] = false;
		pkeys.clear();
	}

	// Append the primary keys, if any were defined.
	if (!pkeys.empty()) {
		statement += ", PRIMARY KEY (";
		for (auto pkey = pkeys.begin(); pkey != pkeys.end();) {
			statement += '`' + std::move(*pkey) + '`';
			if (++pkey != pkeys.end()) {
				statement += ", ";
			}
		}
		statement += ')';
	}

	// Tables implemented by extension can be made read/write; make sure to always
	// keep the rowid column, as we need it to reference rows when handling UPDATE
	// and DELETE queries
	if (is_extension) {
		epilog["WITHOUT ROWID"] = false;
	}

	statement += ')';
	for (auto& ei : epilog) {
		if (ei.second) {
			statement += ' ' + std::move(ei.first);
		}
	}
	return statement;
}

std::string columnDefinition(const PluginResponse& response,
							 bool aliases,
							 bool is_extension)
{
	TableColumns columns;
	// Maintain a map of column to the type, for alias type lookups.
	std::map<std::string, ColumnType> column_types;
	for (const auto& column : response) {
		auto id = column.find("id");
		if (id == column.end()) {
			continue;
		}

		auto cname = column.find("name");
		auto ctype = column.find("type");
		if (id->second == "column" && cname != column.end() &&
			ctype != column.end()) {
			auto options = ColumnOptions::DEFAULT;

			auto cop = column.find("op");
			if (cop != column.end()) {
				auto op = tryTo<int>(cop->second);
				if (op) {
					options = static_cast<ColumnOptions>(op.take());
				}
			}
			auto column_type = columnTypeName(ctype->second);
			columns.push_back(make_tuple(cname->second, column_type, options));
			if (aliases) {
				column_types[cname->second] = column_type;
			}
		} else if (id->second == "columnAlias" && cname != column.end() &&
				   aliases) {
			auto ctarget = column.find("target");
			if (ctarget != column.end()) {
				auto target_ctype = column_types.find(ctarget->second);
				if (target_ctype != column_types.end()) {
					columns.push_back(make_tuple(
										  cname->second, target_ctype->second, ColumnOptions::HIDDEN));
				}
			}
		}
	}
	return columnDefinition(columns, is_extension);
}

ColumnType columnTypeName(const std::string& type)
{
	for (const auto& col : kColumnTypeNames) {
		if (col.second == type) {
			return col.first;
		}
	}
	return UNKNOWN_TYPE;
}

bool ConstraintList::exists(const ConstraintOperatorFlag ops) const
{
	if (ops == ANY_OP) {
		return (constraints_.size() > 0);
	} else {
		for (const struct Constraint& c : constraints_) {
			if (c.op & ops) {
				return true;
			}
		}
		return false;
	}
}

bool ConstraintList::matches(const std::string& expr) const
{
	// Support each SQL affinity type casting.
	if (affinity == TEXT_TYPE) {
		return literal_matches<TEXT_LITERAL>(expr);
	} else if (affinity == INTEGER_TYPE) {
		auto lexpr = tryTo<INTEGER_LITERAL>(expr);
		if (lexpr) {
			return literal_matches<INTEGER_LITERAL>(lexpr.take());
		}
	} else if (affinity == BIGINT_TYPE) {
		auto lexpr = tryTo<BIGINT_LITERAL>(expr);
		if (lexpr) {
			return literal_matches<BIGINT_LITERAL>(lexpr.take());
		}
	} else if (affinity == UNSIGNED_BIGINT_TYPE) {
		auto lexpr = tryTo<UNSIGNED_BIGINT_LITERAL>(expr);
		if (lexpr) {
			return literal_matches<UNSIGNED_BIGINT_LITERAL>(lexpr.take());
		}
	}

	return false;
}

template <typename T>
bool ConstraintList::literal_matches(const T& base_expr) const
{
	bool aggregate = true;
	for (size_t i = 0; i < constraints_.size(); ++i) {
		auto constraint_expr = tryTo<T>(constraints_[i].expr);
		if (!constraint_expr) {
			// Cannot cast input constraint to column type.
			return false;
		}
		if (constraints_[i].op == EQUALS) {
			aggregate = aggregate && (base_expr == constraint_expr.take());
		} else if (constraints_[i].op == GREATER_THAN) {
			aggregate = aggregate && (base_expr > constraint_expr.take());
		} else if (constraints_[i].op == LESS_THAN) {
			aggregate = aggregate && (base_expr < constraint_expr.take());
		} else if (constraints_[i].op == GREATER_THAN_OR_EQUALS) {
			aggregate = aggregate && (base_expr >= constraint_expr.take());
		} else if (constraints_[i].op == LESS_THAN_OR_EQUALS) {
			aggregate = aggregate && (base_expr <= constraint_expr.take());
		} else {
			// Unsupported constraint. Should match every thing.
			return true;
		}
		if (!aggregate) {
			// Speed up comparison.
			return false;
		}
	}
	return true;
}

std::set<std::string> ConstraintList::getAll(ConstraintOperator op) const
{
	std::set<std::string> set;
	for (size_t i = 0; i < constraints_.size(); ++i) {
		if (constraints_[i].op == op) {
			// TODO: this does not apply a distinct.
			set.insert(constraints_[i].expr);
		}
	}
	return set;
}

template <typename T>
std::set<T> ConstraintList::getAll(ConstraintOperator /* op */) const
{
	std::set<T> cs;
	for (const auto& item : constraints_) {
		auto exp = tryTo<T>(item.expr);
		if (exp) {
			cs.insert(exp.take());
		}
	}
	return cs;
}

template <>
std::set<std::string> ConstraintList::getAll(ConstraintOperator op) const
{
	return getAll(op);
}

/// Explicit getAll for INTEGER.
template std::set<INTEGER_LITERAL> ConstraintList::getAll<int>(
	ConstraintOperator) const;

/// Explicit getAll for BIGINT.
template std::set<long long> ConstraintList::getAll<long long>(
	ConstraintOperator) const;

/// Explicit getAll for UNSIGNED_BIGINT.
template std::set<unsigned long long>
ConstraintList::getAll<unsigned long long>(ConstraintOperator) const;

vist::json::Object ConstraintList::serialize() const
{
	// format: { "affinity": "TEXT", "list": [ { "expr": "1", "op": 2 } ] }
	vist::json::Array list;
	for (const auto& constraint : constraints_) {
		vist::json::Object element;
		element["op"] = static_cast<int>(constraint.op);
		element["expr"] =  constraint.expr;
		list.push(element);
	}

	vist::json::Object object;
	object.push("list", list);
	object["affinity"] = columnTypeName(affinity);

	return object;
}

void ConstraintList::deserialize(vist::json::Object& obj)
{
	using namespace vist::json;
	Array list = obj.get<Array>("list");
	for (auto i = 0; i < list.size(); i++) {
		auto element = vist::json::Object::Create(list.at(i));
		int op = element["op"];
		Constraint constraint(static_cast<unsigned char>(op));
		constraint.expr = static_cast<std::string>(element["expr"]);
		this->constraints_.emplace_back(std::move(constraint));
	}

	std::string name = obj.exist("affinity") ? static_cast<std::string>(obj["affinity"]) : "UNKNOWN";
	this->affinity = columnTypeName(name);
}

bool QueryContext::isColumnUsed(const std::string& colName) const
{
	return !colsUsed || colsUsed->find(colName) != colsUsed->end();
}

bool QueryContext::isAnyColumnUsed(
	std::initializer_list<std::string> colNames) const
{
	for (auto& colName : colNames) {
		if (isColumnUsed(colName)) {
			return true;
		}
	}
	return false;
}

void QueryContext::useCache(bool use_cache)
{
	use_cache_ = use_cache;
}

bool QueryContext::useCache() const
{
	return use_cache_;
}

void QueryContext::setCache(const std::string& index,
							const TableRowHolder& cache)
{
	table_->cache[index] = cache->clone();
}

bool QueryContext::isCached(const std::string& index) const
{
	return (table_->cache.count(index) != 0);
}

TableRowHolder QueryContext::getCache(const std::string& index)
{
	return table_->cache[index]->clone();
}

bool QueryContext::hasConstraint(const std::string& column,
								 ConstraintOperator op) const
{
	if (constraints.count(column) == 0) {
		return false;
	}
	return constraints.at(column).exists(op);
}

Status QueryContext::expandConstraints(
	const std::string& column,
	ConstraintOperator op,
	std::set<std::string>& output,
	std::function<Status(const std::string& constraint,
						 std::set<std::string>& output)> predicate)
{
	for (const auto& constraint : constraints[column].getAll(op)) {
		auto status = predicate(constraint, output);
		if (!status) {
			return status;
		}
	}
	return Status(0);
}
} // namespace osquery
