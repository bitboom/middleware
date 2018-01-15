#pragma once

#include <type_traits>

namespace qxx {
namespace condition {

struct Base {};

template<typename L, typename R>
struct And : public Base {
	L l;
	R r;

	And(L l, R r) : l(l), r(r) {}
	operator std::string() const
	{
		return "AND";
	}
};

template<typename L, typename R>
struct Or : public Base {
	L l;
	R r;

	Or(L l, R r) : l(l), r(r) {}
	operator std::string() const
	{
		return "OR";
	}
};

template<typename L, typename R>
struct Binary : public Base {
	L l;
	R r;

	Binary(L l, R r) : l(l), r(r) {}
};

} // namespace condition

template<typename Type>
struct Expression {
	Type value;
};

template<typename Type>
Expression<Type> expr(Type value)
{
	return {value};
}

template<typename L, typename R>
struct Lesser : public condition::Binary<L, R> {
	using condition::Binary<L, R>::Binary;

	operator std::string() const
	{
		return "<";
	}
};

template<typename L, typename R>
Lesser<L, R> operator<(Expression<L> expr, R r)
{
	return {expr.value, r};
}

template<typename L, typename R>
struct Equal : public condition::Binary<L, R>
{
	using condition::Binary<L, R>::Binary;

	operator std::string() const {
		return "=";
	}
};

template<typename L, typename R>
Equal<L, R> operator==(Expression<L> expr, R r)
{
	return {expr.value, r};
}

template<typename L, typename R>
struct Greater : public condition::Binary<L, R>
{
	using condition::Binary<L, R>::Binary;

	operator std::string() const {
		return ">";
	}
};

template<typename L, typename R>
Greater<L, R> operator>(Expression<L> expr, R r)
{
	return {expr.value, r};
}

template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using is_condition = typename std::is_base_of<condition::Base, T>;

template<typename L,
		 typename R,
		 typename = typename qxx::enable_if_t<is_condition<L>::value
							 && qxx::is_condition<R>::value>>
condition::And<L, R> operator&&(const L& l, const R& r)
{
	return {l, r};
}

template<typename L,
		 typename R,
		 typename = typename qxx::enable_if_t<is_condition<L>::value
							 && qxx::is_condition<R>::value>>
condition::Or<L, R> operator||(const L& l, const R& r)
{
	return {l, r};
}

} // namespace qxx
