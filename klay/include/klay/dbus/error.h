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

#ifndef __RUNTIME_DBUS_ERROR_H__
#define __RUNTIME_DBUS_ERROR_H__

#include <gio/gio.h>

#include <klay/klay.h>

namespace klay {
namespace dbus {

class KLAY_EXPORT Error {
public:
	Error();
	~Error();

	GError** operator& ();
	const GError* operator-> () const;
	operator bool () const;

private:
	GError* error;
};

} // namespace dbus
} // namespace klay

namespace dbus = klay::dbus;

#endif //!__RUNTIME_DBUS_ERROR_H__
