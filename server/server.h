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

#ifndef __DPM_SERVER_H__
#define __DPM_SERVER_H__

#include <string>
#include <memory>

#include "policy-manager.h"
#include "client-manager.h"

#include <klay/filesystem.h>
#include <klay/file-descriptor.h>
#include <klay/rmi/service.h>

class Server {
public:
	Server();
	~Server();

	void run();
	void terminate();

	template<typename Type, typename... Args>
	void setMethodHandler(const std::string& privilege, const std::string& method,
						  const typename rmi::MethodHandler<Type, Args...>::type& handler)
	{
		service->setMethodHandler<Type, Args...>(privilege, method, handler);
	}

	template <typename... Args>
	void notify(const std::string& name, Args&&... args)
	{
		service->notify<Args...>(name, std::forward<Args>(args)...);
	}

	uid_t getPeerUid() const
	{
		return service->getPeerUid();
	}

	gid_t getPeerGid() const
	{
		return service->getPeerGid();
	}

	pid_t getPeerPid() const
	{
		return service->getPeerPid();
	}

	void createNotification(const std::string& name)
	{
		service->createNotification(name);
	}

	void createNotification(const std::vector<std::string>& notification)
	{
		for (const std::string& name : notification) {
			service->createNotification(name);
		}
	}

	template<typename DataType>
	bool setPolicy(const std::string& name, const DataType& value, const std::string& event, const std::string& info);

	template<typename DataType>
	bool setPolicy(const std::string& name, const DataType& value, const std::string& info)
	{
		return setPolicy<DataType>(name, value, name, info);
	}

	template<typename DataType>
	DataType getPolicy(const std::string& name, uid_t uid)
	{
		return policyManager->getUserPolicy<DataType>(name, uid);
	}

	template<typename DataType>
	DataType getPolicy(const std::string& name)
	{
		return policyManager->getGlobalPolicy<DataType>(name);
	}

	PolicyManager& getPolicyManager()
	{
		return *policyManager;
	}

private:
	runtime::FileDescriptor registerNotificationSubscriber(const std::string& name);
	int unregisterNotificationSubscriber(const std::string& name, int id);

	bool checkPeerPrivilege(const rmi::Credentials& cred, const std::string& privilege);

private:
	std::string securityLabel;
	std::string location;

	std::unique_ptr<PolicyManager> policyManager;
	std::unique_ptr<rmi::Service> service;
};

template<typename DataType>
bool Server::setPolicy(const std::string& name, const DataType& value, const std::string& event, const std::string& info)
{
	DeviceAdministrator admin(getPeerPid(), getPeerUid());
	if (policyManager->setPolicy<DataType>(admin, name, value)) {
		if (event.empty() == false) {
			service->notify(event, info);
		}
		return true;
	}

	return false;
}
#endif //__DPM_SERVER_H__
