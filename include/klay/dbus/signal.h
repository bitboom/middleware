/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __RUNTIME_DBUS_SIGNAL_H__
#define __RUNTIME_DBUS_SIGNAL_H__


#include <string>
#include <utility>

#include <klay/klay.h>
#include <klay/dbus/connection.h>

namespace klay {
namespace dbus {
namespace signal {

using SignalCallback = dbus::Connection::SignalCallback;

class KLAY_EXPORT Sender {
public:
	explicit Sender(const std::string &objectPath, const std::string &interfaceName);
	virtual ~Sender(void) = default;

	void setBusName(const std::string &name);
	void setInterfaceName(const std::string &name);

	void addSignal(const std::string &manifestPath,
				   const std::string &signalName,
				   const std::string &argumentType) const;

	template<typename... Arguments>
	void emit(const std::string &signalName,
			  const std::string &argumentType,
			  Arguments&&... argumnets) const;

private:
	std::string busName;
	std::string objectPath;
	std::string interfaceName;
};

class KLAY_EXPORT Receiver {
public:
	explicit Receiver(const std::string &objectPath, const std::string &interfaceName);
	virtual ~Receiver(void) = default;

	void setInterfaceName(const std::string &name);
	void setSenderName(const std::string &name);

	unsigned int subscribe(const std::string &signalName,
						   const SignalCallback& signalCallback) const;
	void unsubscribe(unsigned int id) const;

private:
	std::string objectPath;
	std::string interfaceName;
	std::string senderName;
};

template<typename... Arguments>
void Sender::emit(const std::string &signalName,
				  const std::string &argumentType,
				  Arguments&&... argumnets) const
{
	dbus::Connection &conn = dbus::Connection::getSystem();
	conn.emitSignal(busName,
					objectPath,
					interfaceName,
					signalName,
					argumentType,
					std::forward<Arguments>(argumnets)...);
}

} // namespace signal
} // namespace dbus
} // namespace klay

namespace dbus = klay::dbus;

#endif //! __RUNTIME_DBUS_SIGNAL_H__
