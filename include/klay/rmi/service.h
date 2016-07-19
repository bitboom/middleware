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

#ifndef __RMI_SERVICE_H__
#define __RMI_SERVICE_H__

#include <list>
#include <mutex>
#include <memory>
#include <vector>
#include <thread>
#include <string>
#include <functional>
#include <unordered_map>

#include <klay/mainloop.h>
#include <klay/thread-pool.h>
#include <klay/preprocessor.h>
#include <klay/rmi/message.h>
#include <klay/rmi/connection.h>
#include <klay/rmi/notification.h>
#include <klay/rmi/callback-holder.h>

#define STRIP_(...)
#define STRIP(x)         STRIP_ x

#define TYPEOF____(...)   __VA_ARGS__
#define TYPEOF___(...)   (__VA_ARGS__),
#define TYPEOF__(x, ...) TYPEOF____ x
#define TYPEOF_(...)     TYPEOF__(__VA_ARGS__)
#define TYPEOF(x)        TYPEOF_(TYPEOF___ x, )

#define PLACEHOLDER(n)   std::placeholders::_##n

#define PROTOTYPE_(D, N) SEQUENCE(D, N)
#define PROTOTYPE(...)   PROTOTYPE_(PLACEHOLDER, VAR_ARGS_SIZE(__VA_ARGS__))

#define registerMethod(T, P, M, ...)                                          \
setMethodHandler<TYPEOF(M), TYPEOF(STRIP(STRIP(M)))>                          \
				(P, STRINGIFY(TYPEOF(STRIP(M))), std::bind(&TYPEOF(STRIP(M)), T, \
				PROTOTYPE(TYPEOF(STRIP(STRIP(M))))))

#define registerParametricMethod(T, P, M, ...)                                \
setMethodHandler<TYPEOF(M), TYPEOF(STRIP(STRIP(M)))>                          \
				(P, STRINGIFY(TYPEOF(STRIP(M))), std::bind(&TYPEOF(STRIP(M)), T, \
				PROTOTYPE(TYPEOF(STRIP(STRIP(M))))))

#define registerNonparametricMethod(T, P, M, ...)                             \
setMethodHandler<TYPEOF(M)>                                                   \
				(P, STRINGIFY(TYPEOF(STRIP(M))), std::bind(&TYPEOF(STRIP(M)), T))

namespace rmi {

typedef std::function<bool(const Connection& connection)> ConnectionCallback;
typedef std::function<bool(const Credentials& cred, const std::string& privilege)> PrivilegeChecker;

class Service {
public:
	Service(const std::string& address);
	~Service();

	Service(const Service&) = delete;
	Service& operator=(const Service&) = delete;

	void start(bool useGMainloop = false);
	void stop();

	void setPrivilegeChecker(const PrivilegeChecker& checker);
	void setNewConnectionCallback(const ConnectionCallback& callback);
	void setCloseConnectionCallback(const ConnectionCallback& callback);

	template<typename Type, typename... Args>
	void setMethodHandler(const std::string& privilege, const std::string& method,
						  const typename MethodHandler<Type, Args...>::type& handler);

	void createNotification(const std::string& name);
	int subscribeNotification(const std::string& name);
	int unsubscribeNotification(const std::string& name, const int id);

	template <typename... Args>
	void notify(const std::string& name, Args&&... args);

	pid_t getPeerPid() const
	{
		return processingContext.credentials.pid;
	}

	uid_t getPeerUid() const
	{
		return processingContext.credentials.uid;
	}

	gid_t getPeerGid() const
	{
		return processingContext.credentials.gid;
	}

private:
	struct ProcessingContext {
		ProcessingContext() = default;
		ProcessingContext(const std::shared_ptr<Connection>& connection) :
			credentials(connection->getPeerCredentials())
		{
		}

		Credentials credentials;
	};

	typedef std::list<std::shared_ptr<Connection>> ConnectionRegistry;
	typedef std::function<void(const std::shared_ptr<Connection>& connection)> CallbackDispatcher;

	typedef std::function<Message(Message& message)> MethodDispatcher;

	struct MethodContext {
		MethodContext(const std::string& priv, MethodDispatcher&& disp) :
			privilege(priv), dispatcher(std::move(disp))
		{
		}

		std::string privilege;
		MethodDispatcher dispatcher;
	};

	typedef std::unordered_map<std::string, std::shared_ptr<MethodContext>> MethodRegistry;
	typedef std::unordered_map<std::string, Notification> NotificationRegistry;

	void onMessageProcess(const std::shared_ptr<Connection>& connection);

	ConnectionRegistry::iterator getConnectionIterator(const int id);

	CallbackDispatcher onNewConnection;
	CallbackDispatcher onCloseConnection;
	PrivilegeChecker onMethodCall;

	MethodRegistry methodRegistry;
	NotificationRegistry notificationRegistry;
	ConnectionRegistry connectionRegistry;

	runtime::Mainloop mainloop;
	std::string address;

	runtime::ThreadPool workqueue;
	std::mutex stateLock;
	std::mutex notificationLock;
	std::mutex methodRegistryLock;

	static thread_local ProcessingContext processingContext;
};

template<typename Type, typename... Args>
void Service::setMethodHandler(const std::string& privilege, const std::string& method,
							   const typename MethodHandler<Type, Args...>::type& handler)
{
	auto dispatchMethod = [handler](Message& message) {
		CallbackHolder<Type, Args...> callback(handler);
		Message reply = message.createReplyMessage();
		reply.packParameters<Type>(callback.dispatch(message));

		return reply;
	};

	std::lock_guard<std::mutex> lock(methodRegistryLock);

	if (methodRegistry.count(method)) {
		throw runtime::Exception("Method handler already registered");
	}

	methodRegistry[method] = std::make_shared<MethodContext>(privilege, dispatchMethod);
}

template <typename... Args>
void Service::notify(const std::string& name, Args&&... args)
{
	std::lock_guard<std::mutex> lock(notificationLock);

	Notification& slot = notificationRegistry.at(name);
	slot.notify(name, std::forward<Args>(args)...);
}

} // namespace rmi

#endif //__RMI_SERVICE_H__
