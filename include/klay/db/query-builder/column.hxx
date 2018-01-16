#pragma once

#include <string>
#include <tuple>

namespace qxx {

template<typename Object, typename Field>
struct Column {
	typedef Field Object::*Type;

	std::string name;
	Type type;
};

template<typename O, typename F>
Column<O, F> make_column(const std::string& name, F O::*field) {
	return {name, field};
}

template<typename Type>
struct Distinct {
	Type value;
};

template<typename... Args>
auto distinct(Args&&... args) -> decltype(Distinct<std::tuple<Args...>>())
{
	return {std::make_tuple(std::forward<Args>(args)...)};
}

} // namespace qxx
