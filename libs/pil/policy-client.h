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

#ifndef __POLICY_CLIENT_H__
#define __POLICY_CLIENT_H__

#include <string>
#include <memory>
#include <functional>
#include <cerrno>
#include <vconf.h>

#include <klay/rmi/method.h>

#include "status.h"

typedef std::function<void(const char*, const char*, void*)> SignalHandler;

class DevicePolicyClient {
public:
	DevicePolicyClient() noexcept;
	~DevicePolicyClient() noexcept;

	int subscribeSignal(const std::string& name,
						const SignalHandler& handler,
						void* data) noexcept;
	int unsubscribeSignal(int subscriberId) noexcept;

	template<typename Type, typename... Args>
	Type methodCall(const std::string& method, Args&&... args)
	{
		if (!maintenanceMode) {
			errno = EPROTONOSUPPORT;
			return Type();
		}

		rmi::Connection conn(clientAddress);
		Type ret = rmi::RemoteMethod<>::invoke<Type, Args...>(conn, method, std::forward<Args>(args)...);
		return ret;
	}

	template<typename Type, typename... Args>
	Type methodCallForce(const std::string& method, Args&&... args)
	{
		rmi::Connection conn(clientAddress);
		Type ret = rmi::RemoteMethod<>::invoke<Type, Args...>(conn, method, std::forward<Args>(args)...);
		return ret;
	}

private:
	static void maintenanceModeDispatcher(keynode_t *node, void *data);
private:
	int maintenanceMode;
	std::string clientAddress;
};

DevicePolicyClient& GetDevicePolicyClient(void* handle);

#endif //__POLICY_CLIENT_H__
