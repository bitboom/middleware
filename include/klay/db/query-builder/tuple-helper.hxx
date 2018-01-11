#pragma once

#include <tuple>

namespace qxx {
namespace tuple_helper {
namespace internal {

template<int n, typename T, typename C>
class Iterator {
public:
	Iterator(const T& tuple, C&& closure) {
		Iterator<n - 1, T, C> iter(tuple, std::forward<C>(closure));
		closure(std::get<n-1>(tuple));
	}
};

template<typename T, typename C>
class Iterator<0, T, C> {
public:
	Iterator(const T&, C&&) {}
};

} // namespace internal

template<typename T, typename C>
void for_each(const T& tuple, C&& closure)
{
	using Iter = internal::Iterator<std::tuple_size<T>::value, T, C>;
	Iter iter(tuple, std::forward<C>(closure));
}

} // namspace tuple-hepler
} // namspace qxx
