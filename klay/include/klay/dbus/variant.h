/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __RUNTIME_DBUS_VARIANT_H__
#define __RUNTIME_DBUS_VARIANT_H__

#include <gio/gio.h>

#include <string>

#include <klay/klay.h>

namespace klay {
namespace dbus {

class KLAY_EXPORT Variant {
public:
	Variant();
	Variant(GVariant* var);
	Variant(Variant&& var);
	Variant(const std::string& format, ...);
	~Variant();

	Variant& operator=(GVariant* var);
	operator bool () const;
	GVariant* operator & ();

	void get(const std::string& format, ...) const;
private:
	GVariant* variant;
};

class KLAY_EXPORT VariantIterator {
public:
	VariantIterator(GVariantIter* it);
	VariantIterator(VariantIterator&& it);
	VariantIterator();
	~VariantIterator();

	VariantIterator& operator=(GVariantIter* it);
	operator bool () const;
	GVariantIter** operator & ();

	bool get(const std::string& format, ...) const;
private:
	GVariantIter* iterator;
};

} // namespace dbus
} // namespace klay

namespace dbus = klay::dbus;

#endif //!__RUNTIME_DBUS_VARIANT_H__
