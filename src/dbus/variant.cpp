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
#include <klay/dbus/variant.h>

namespace dbus {

Variant::Variant(GVariant* var) :
	variant(var)
{
}

Variant::Variant(Variant&& var) :
	variant(var.variant)
{
	var.variant = nullptr;
}

Variant::Variant() :
	variant(nullptr)
{
}

Variant::Variant(const std::string& format, ...)
{
	va_list ap;

	va_start(ap, format);
	variant = g_variant_new_va(format.c_str(), NULL, &ap);
	va_end(ap);
}

Variant::~Variant()
{
	/*
	 * variant is foating
	 * if (variant) {
	 *     g_variant_unref(variant);
	 * }
	 */
}

Variant& Variant::operator=(GVariant* var)
{
	variant = var;
	return *this;
}

GVariant* Variant::operator & ()
{
	return variant;
}

Variant::operator bool () const
{
	return variant != nullptr;
}

void Variant::get(const std::string& format, ...) const
{
	va_list ap;

	va_start(ap, format);
	g_variant_get_va(variant, format.c_str(), NULL, &ap);
	va_end(ap);
}


VariantIterator::VariantIterator(GVariantIter* it) :
	iterator(it)
{
}

VariantIterator::VariantIterator(VariantIterator&& it) :
	iterator(it.iterator)
{
	it.iterator = nullptr;
}

VariantIterator::VariantIterator() :
	iterator(nullptr)
{
}

VariantIterator::~VariantIterator()
{
	if (iterator) {
		g_variant_iter_free(iterator);
	}
}

VariantIterator& VariantIterator::operator=(GVariantIter* it)
{
	iterator = it;
	return *this;
}

VariantIterator::operator bool () const
{
	return iterator != nullptr;
}

GVariantIter** VariantIterator::operator & ()
{
	return &iterator;
}

bool VariantIterator::get(const std::string& format, ...) const
{
	GVariant *var;
	va_list ap;
	var = g_variant_iter_next_value(iterator);
	if (var == NULL) {
		return false;
	}

	va_start(ap, format);
	g_variant_get_va(var, format.c_str(), NULL, &ap);
	va_end(ap);

	g_variant_unref(var);

	return true;
}

} // namespace dbus
