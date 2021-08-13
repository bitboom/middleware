/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        singleton_impl.h
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of singleton
 */
#ifndef DPL_SINGLETON_IMPL_H
#define DPL_SINGLETON_IMPL_H

#include <dpl/singleton.h>

/*
 * WARNING!
 *
 * If some singleton's implementation uses another singletons implementation,
 * those templates make the second singleton a dubleton. Be warned. Try to use
 * singleton_safe_impl.h if possible.
 */

namespace VcoreDPL {
template<typename Class>
Singleton<Class> &Singleton<Class>::InternalInstance()
{
	static Singleton<Class> instance;
	return instance;
}

template<typename Class>
Class &Singleton<Class>::Instance()
{
	Singleton<Class> &instance = Singleton<Class>::InternalInstance();
	return instance;
}
} // namespace VcoreDPL

#define IMPLEMENT_SINGLETON(Type)                                                     \
	template VcoreDPL::Singleton<Type>&VcoreDPL::Singleton<Type>::InternalInstance(); \
	template Type & VcoreDPL::Singleton<Type>::Instance();                            \

#endif // DPL_SINGLETON_IMPL_H
