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

#include <klay/rmi/client.h>

typedef std::function<void(const char*, const char*, void*)> PolicyChangeListener;
typedef std::function<void(const char*, const char*, void*)> SignalListener;

class DevicePolicyContext {
public:
	typedef DevicePolicyContext PolicyControlContext;

	DevicePolicyContext() noexcept;
	~DevicePolicyContext() noexcept;

	int connect() noexcept;
	int connect(const std::string& address) noexcept;
	void disconnect() noexcept;

	int subscribePolicyChange(const std::string& name, const PolicyChangeListener& listener, void* data);
	int unsubscribePolicyChange(int subscriberId);

	int subscribeSignal(const std::string& name, const SignalListener& listener, void* data);
	int unsubscribeSignal(int subscriberId);

	template<typename Policy, typename... Args>
	Policy createPolicyInterface(Args&&... args) noexcept
	{
		return Policy(getPolicyControlContext(), std::forward<Args>(args)...);
	}

	template<typename Type, typename... Args>
	Type methodCall(const std::string& method, Args&&... args)
	{
		if (maintenanceMode) {
			return client->methodCall<Type, Args...>(method, std::forward<Args>(args)...);
		}

		errno = EPROTONOSUPPORT;
		return Type();
	}

private:
	PolicyControlContext& getPolicyControlContext()
	{
		return *this;
	}

private:
	int maintenanceMode;
	std::unique_ptr<rmi::Client> client;
};

DevicePolicyContext& GetDevicePolicyContext(void* handle);

#endif //__POLICY_CLIENT_H__
