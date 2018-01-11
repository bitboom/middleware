#pragma once

#include <string>
#include <vector>

namespace qxx {
namespace internal {

template<typename... Base>
class TableImpl {
public:
	std::vector<std::string> getColumnNames() const noexcept { return {}; }

	template<class Type>
	std::string getColumnName(Type type) const noexcept { return std::string(); }

	int size() const noexcept { return 0; }
};

template<typename Front, typename... Rest>
class TableImpl<Front, Rest...> : public TableImpl<Rest...> {
public:
	using Column = Front;

	explicit TableImpl(Front front, Rest ...rest) : Base(rest...), column(front) {}

	int size() const noexcept { return Base::size() + 1; }

	std::vector<std::string> getColumnNames(void) const noexcept {
		auto names = Base::getColumnNames();
		names.emplace_back(this->column.name);
		return names;
	}

	template<typename ColumnType>
	std::string getColumnName(ColumnType type) const noexcept {
		// [TO-DO] Do Not Cast.
		// [ALTER] std::is_same<F, typename T::field_type>{}
		// [PROBLEM] Cannot multi-table select..
		// [CRITICAL] (&Data::int == &Data2::int) is same
		if (reinterpret_cast<ColumnType>(column.type) == type)
			return column.name;

		return Base::template getColumnName<ColumnType>(type);
	}

private:
	using Base = TableImpl<Rest...>;

	Column column;
};

} // namespace internal
} // namespace qxx
