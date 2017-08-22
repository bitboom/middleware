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

#define ARGSEQ_0(how)
#define ARGSEQ_1(how)     ARGSEQ_0(how), how(1)
#define ARGSEQ_2(how)     ARGSEQ_1(how), how(2)
#define ARGSEQ_3(how)     ARGSEQ_2(how), how(3)
#define ARGSEQ_4(how)     ARGSEQ_3(how), how(4)
#define ARGSEQ_5(how)     ARGSEQ_4(how), how(5)
#define ARGSEQ_6(how)     ARGSEQ_5(how), how(6)
#define ARGSEQ_7(how)     ARGSEQ_6(how), how(7)
#define ARGSEQ_8(how)     ARGSEQ_7(how), how(8)
#define ARGSEQ_9(how)     ARGSEQ_8(how), how(9)
#define ARGSEQ_10(how)    ARGSEQ_9(how), how(10)
#define ARGSEQ_11(how)    ARGSEQ_10(how), how(11)
#define ARGSEQ_12(how)    ARGSEQ_11(how), how(12)
#define ARGSEQ_13(how)    ARGSEQ_12(how), how(13)
#define ARGSEQ_14(how)    ARGSEQ_13(how), how(14)
#define ARGSEQ_15(how)    ARGSEQ_14(how), how(15)
#define ARGSEQ_16(how)    ARGSEQ_15(how), how(16)
#define ARGSEQ_17(how)    ARGSEQ_16(how), how(17)
#define ARGSEQ_18(how)    ARGSEQ_17(how), how(18)
#define ARGSEQ_19(how)    ARGSEQ_18(how), how(19)
#define ARGSEQ_20(how)    ARGSEQ_19(how), how(20)
#define ARGSEQ_21(how)    ARGSEQ_20(how), how(21)
#define ARGSEQ_22(how)    ARGSEQ_21(how), how(22)
#define ARGSEQ_23(how)    ARGSEQ_22(how), how(23)
#define ARGSEQ_24(how)    ARGSEQ_23(how), how(24)
#define ARGSEQ_25(how)    ARGSEQ_24(how), how(25)
#define ARGSEQ_26(how)    ARGSEQ_25(how), how(26)
#define ARGSEQ_27(how)    ARGSEQ_26(how), how(27)
#define ARGSEQ_28(how)    ARGSEQ_27(how), how(28)
#define ARGSEQ_29(how)    ARGSEQ_28(how), how(29)
#define ARGSEQ_30(how)    ARGSEQ_29(how), how(30)
#define ARGSEQ_31(how)    ARGSEQ_30(how), how(31)
#define ARGSEQ_32(how)    ARGSEQ_31(how), how(32)
#define ARGSEQ_N(how, N)  ARGSEQ_##N(how)

#define PLACEHOLDER(n)   std::placeholders::_##n

#define PROTOTYPE_(D, N) ARGSEQ_N(D, N)
#define PROTOTYPE(...)   PROTOTYPE_(PLACEHOLDER, VAR_ARGS_SIZE(__VA_ARGS__))

#define SEPSEQ_0()
#define SEPSEQ_1()    ,
#define SEPSEQ_2()    SEPSEQ_1()
#define SEPSEQ_3()    SEPSEQ_2()
#define SEPSEQ_4()    SEPSEQ_3()
#define SEPSEQ_5()    SEPSEQ_4()
#define SEPSEQ_6()    SEPSEQ_5()
#define SEPSEQ_7()    SEPSEQ_6()
#define SEPSEQ_8()    SEPSEQ_7()
#define SEPSEQ_9()    SEPSEQ_8()
#define SEPSEQ_10()   SEPSEQ_9()
#define SEPSEQ_11()   SEPSEQ_10()
#define SEPSEQ_12()   SEPSEQ_11()
#define SEPSEQ_13()   SEPSEQ_12()
#define SEPSEQ_14()   SEPSEQ_13()
#define SEPSEQ_15()   SEPSEQ_14()
#define SEPSEQ_16()   SEPSEQ_15()
#define SEPSEQ_17()   SEPSEQ_16()
#define SEPSEQ_18()   SEPSEQ_17()
#define SEPSEQ_19()   SEPSEQ_18()
#define SEPSEQ_20()   SEPSEQ_19()
#define SEPSEQ_21()   SEPSEQ_20()
#define SEPSEQ_22()   SEPSEQ_21()
#define SEPSEQ_23()   SEPSEQ_22()
#define SEPSEQ_24()   SEPSEQ_23()
#define SEPSEQ_25()   SEPSEQ_24()
#define SEPSEQ_26()   SEPSEQ_25()
#define SEPSEQ_27()   SEPSEQ_26()
#define SEPSEQ_28()   SEPSEQ_27()
#define SEPSEQ_29()   SEPSEQ_28()
#define SEPSEQ_30()   SEPSEQ_29()
#define SEPSEQ_31()   SEPSEQ_30()
#define SEPSEQ_32()   SEPSEQ_31()
#define SEPSEQ_N(N)   SEPSEQ_##N()

#define SEPSEQ_(N)    SEPSEQ_N(N)
#define SEPSEQ(...)   SEPSEQ_(VAR_ARGS_SIZE(__VA_ARGS__))

#define expose(T, P, M, ...)                                                    \
setMethodHandler<TYPEOF(M) SEPSEQ(TYPEOF(STRIP(STRIP(M)))) TYPEOF(STRIP(STRIP(M)))>  \
				(P, STRINGIFY(TYPEOF(STRIP(M))), std::bind(&TYPEOF(STRIP(M)), T PROTOTYPE(TYPEOF(STRIP(STRIP(M))))))

#define registerMethod(T, P, M, ...)                                          \
setMethodHandler<TYPEOF(M), TYPEOF(STRIP(STRIP(M)))>                          \
				(P, STRINGIFY(TYPEOF(STRIP(M))), std::bind(&TYPEOF(STRIP(M)), T \
				PROTOTYPE(TYPEOF(STRIP(STRIP(M))))))

#define registerParametricMethod(T, P, M, ...)                                \
setMethodHandler<TYPEOF(M), TYPEOF(STRIP(STRIP(M)))>                          \
				(P, STRINGIFY(TYPEOF(STRIP(M))), std::bind(&TYPEOF(STRIP(M)), T \
				PROTOTYPE(TYPEOF(STRIP(STRIP(M))))))

#define registerNonparametricMethod(T, P, M, ...)                             \
setMethodHandler<TYPEOF(M)>                                                   \
				(P, STRINGIFY(TYPEOF(STRIP(M))), std::bind(&TYPEOF(STRIP(M)), T))

namespace rmi {

typedef std::function<bool(const Connection& connection)> ConnectionCallback;
typedef std::function<bool(const Credentials& cred, const std::string& privilege)> PrivilegeChecker;
typedef std::function<void(const Credentials& cred, const std::string& method, int condition)> AuditTrail;

class Service {
public:
	Service(const std::string& address);
	virtual ~Service();

	Service(const Service&) = delete;
	Service& operator=(const Service&) = delete;

	void start();
	void stop();

	void setAuditTrail(const AuditTrail& trail);
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

	static pid_t getPeerPid()
	{
		return processingContext.credentials.pid;
	}

	static uid_t getPeerUid()
	{
		return processingContext.credentials.uid;
	}

	static gid_t getPeerGid()
	{
		return processingContext.credentials.gid;
	}

	runtime::Mainloop mainloop;

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
	PrivilegeChecker onPrivilegeCheck;
	AuditTrail onAuditTrail;

	MethodRegistry methodRegistry;
	NotificationRegistry notificationRegistry;
	ConnectionRegistry connectionRegistry;

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
