/*
 *  Copyright (c) 2018-present Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file   functor.hpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Functor is callable object which binds instance with member function.
 */

#pragma once

#include <vist/archive.hpp>
#include <vist/klass/function.hpp>

#include <functional>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace vist {
namespace klass {

struct AbstractFunctor {
	template<typename R, typename...Args>
	R invoke(Args&&... args);
	inline Archive invoke(Archive& archive);

protected:
	virtual Archive dispatch(Archive& archive) = 0;
};

using FunctorMap = std::unordered_map<std::string, std::shared_ptr<AbstractFunctor>>;

template<typename R, typename K, typename... Ps>
class Functor : public AbstractFunctor {
public:
	using Klass = K;
	using MemFunc = Function<R, K, Ps...>;
	using Invokable = std::function<R(K&, Ps...)>;

	explicit Functor(std::shared_ptr<Klass> instance, MemFunc memFunc);

	template<typename... Args>
	auto operator()(Args&&... args) -> typename MemFunc::Return;
	inline auto operator()(Archive& archive) -> typename MemFunc::Return;

protected:
	inline Archive dispatch(Archive& archive) override;

private:
	template<typename T>
	auto operator()(T& tuple, EmptySequence) -> typename MemFunc::Return;
	template<typename T, std::size_t... I>
	auto operator()(T& tuple, IndexSequence<I...>) -> typename MemFunc::Return;

	std::shared_ptr<Klass> instance;
	MemFunc memFunc;
};

template<typename R, typename...Args>
R AbstractFunctor::invoke(Args&&... args)
{
	Archive parameters;
	parameters.pack(std::forward<Args>(args)...);

	auto result = this->dispatch(parameters);
	R ret;
	result >> ret;
	return ret;
}

Archive AbstractFunctor::invoke(Archive& archive)
{
	return this->dispatch(archive);
}

template<typename R, typename K, typename... Ps>
Functor<R, K, Ps...>::Functor(std::shared_ptr<Klass> instance, MemFunc memFunc)
	: instance(instance), memFunc(std::move(memFunc))
{
}

template<typename R, typename K, typename... Ps>
template<typename... Args>
auto Functor<R, K, Ps...>::operator()(Args&&... args) -> typename MemFunc::Return
{
	const Invokable& invokable = this->memFunc.get();
	return invokable(*this->instance, std::forward<Args>(args)...);
}

template<typename R, typename K, typename... Ps>
auto Functor<R, K, Ps...>::operator()(Archive& archive) -> typename MemFunc::Return
{
	using ParamsTuple = typename MemFunc::Parameters;
	constexpr auto size = std::tuple_size<ParamsTuple>::value;

	ParamsTuple params;
	archive.transform(params);

	return (*this)(params, make_index_sequence<size>());
}

template<typename R, typename K, typename... Ps>
Archive Functor<R, K, Ps...>::dispatch(Archive& archive)
{
	Archive ret;
	return (ret << (*this)(archive));
}

template<typename R, typename K, typename... Ps>
template<typename T>
auto Functor<R, K, Ps...>::operator()(T& tuple,
									  EmptySequence) -> typename MemFunc::Return
{
	return (*this)();
}

template<typename R, typename K, typename... Ps>
template<typename T, std::size_t... I>
auto Functor<R, K, Ps...>::operator()(T& tuple,
									  IndexSequence<I...>) -> typename MemFunc::Return
{
	return (*this)(std::get<I>(tuple)...);
}

template<typename R, typename K, typename... Ps>
Functor<R, K, Ps...> make_functor(std::shared_ptr<K> instance, R (K::* member)(Ps...))
{
	if (instance == nullptr)
		throw std::invalid_argument("Instance can't be nullptr.");

	return Functor<R, K, Ps...>(instance, make_function(member));
}

template<typename R, typename K, typename... Ps>
std::shared_ptr<Functor<R, K, Ps...>> make_functor_ptr(K* instance,
													   R (K::* member)(Ps...))
{
	if (instance == nullptr)
		throw std::invalid_argument("Instance can't be nullptr.");

	std::shared_ptr<K> smartPtr(instance);
	return std::make_shared<Functor<R, K, Ps...>>(smartPtr, make_function(member));
}

template<typename R, typename K, typename... Ps>
std::shared_ptr<Functor<R, K, Ps...>> make_functor_ptr(std::shared_ptr<K> instance,
													   R (K::* member)(Ps...))
{
	if (instance == nullptr)
		throw std::invalid_argument("Instance can't be nullptr.");

	return std::make_shared<Functor<R, K, Ps...>>(instance, make_function(member));
}

} // namespace klass
} // namespace vist
