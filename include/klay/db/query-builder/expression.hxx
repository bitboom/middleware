#pragma once

namespace qxx {
namespace condition {

template<class L, class R>
struct Binary {
	L l;
	R r;

	Binary(L l, R r) : l(l), r(r) {}
};

} // namespace condition

template<class Type>
struct Expression {
	Type value;
};

template<class Type>
Expression<Type> expr(Type value) {
	return {value};
}

template<class L, class R>
struct Lesser : public condition::Binary<L, R> {
	using condition::Binary<L, R>::Binary;

	operator std::string() const {
		return "<";
	}
};

template<class L, class R>
Lesser<L, R> operator<(Expression<L> expr, R r) {
	return {expr.value, r};
}

template<class L, class R>
struct Equal : public condition::Binary<L, R> {
	using condition::Binary<L, R>::Binary;

	operator std::string() const {
		return "=";
	}
};

template<class L, class R>
Equal<L, R> operator==(Expression<L> expr, R r) {
	return {expr.value, r};
}

template<class L, class R>
struct Greater : public condition::Binary<L, R> {
	using condition::Binary<L, R>::Binary;

	operator std::string() const {
		return ">";
	}
};

template<class L, class R>
Equal<L, R> operator>(Expression<L> expr, R r) {
	return {expr.value, r};
}

} // namespace qxx
