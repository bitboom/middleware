/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <sys/types.h>

#include <algorithm>

#include <klay/exception.h>
#include <klay/rmi/service.h>
#include <klay/rmi/message.h>
#include <klay/audit/logger.h>

namespace rmi {

thread_local Service::ProcessingContext Service::processingContext;

Service::Service(const std::string& path) :
	address(path), workqueue(5)
{
	setNewConnectionCallback(nullptr);
	setCloseConnectionCallback(nullptr);

	onPrivilegeCheck = [](const Credentials& cred, const std::string& privilege) {
		return true;
	};

	onAuditTrail = [](const Credentials& cred, const std::string& name, int condition) {
	};
}

Service::~Service()
{
}

void Service::start()
{
	Socket socket(Socket::create(address));

	auto accept = [&](int fd, runtime::Mainloop::Event event) {
		onNewConnection(std::make_shared<Connection>(socket.accept()));
	};

	mainloop.addEventSource(socket.getFd(),
							EPOLLIN | EPOLLHUP | EPOLLRDHUP,
							accept);

	mainloop.run();
}

void Service::stop()
{
}

Service::ConnectionRegistry::iterator Service::getConnectionIterator(const int id)
{
	return std::find_if(connectionRegistry.begin(), connectionRegistry.end(),
						[id](const std::shared_ptr<Connection>& connection) {
		return id == connection->getFd();
	});
}

void Service::setPrivilegeChecker(const PrivilegeChecker& checker)
{
	onPrivilegeCheck = std::move(checker);
}

void Service::setAuditTrail(const AuditTrail& trail)
{
	onAuditTrail = std::move(trail);
}

void Service::setNewConnectionCallback(const ConnectionCallback& connectionCallback)
{
	auto callback = [connectionCallback, this](const std::shared_ptr<Connection>& connection) {
		auto handle = [&](int fd, runtime::Mainloop::Event event) {
			std::lock_guard<std::mutex> lock(stateLock);

			auto iter = getConnectionIterator(fd);
			if (iter == connectionRegistry.end()) {
				return;
			}

			if ((event & EPOLLHUP) || (event & EPOLLRDHUP)) {
				onCloseConnection(*iter);
				connectionRegistry.erase(iter);
				return;
			}

			onMessageProcess(*iter);
		};

		if ((connectionCallback == nullptr) ||
			(connectionCallback(*connection) == true)) {
			mainloop.addEventSource(connection->getFd(),
									EPOLLIN | EPOLLHUP | EPOLLRDHUP,
									handle);
			std::lock_guard<std::mutex> lock(stateLock);
			connectionRegistry.push_back(connection);
		}
	};

	std::lock_guard<std::mutex> lock(stateLock);
	onNewConnection = std::move(callback);
}

void Service::setCloseConnectionCallback(const ConnectionCallback& closeCallback)
{
	auto callback = [closeCallback, this](const std::shared_ptr<Connection>& connection) {
		mainloop.removeEventSource(connection->getFd());
		if (closeCallback) {
			closeCallback(*connection);
		}
	};

	std::lock_guard<std::mutex> lock(stateLock);
	onCloseConnection = std::move(callback);
}

void Service::createNotification(const std::string& name)
{
	std::lock_guard<std::mutex> lock(notificationLock);

	if (notificationRegistry.count(name)) {
		throw runtime::Exception("Notification already registered");
	}

	notificationRegistry.emplace(name, Notification(name));
}

int Service::subscribeNotification(const std::string& name)
{
	auto closeHandler = [&, name, this](int fd, runtime::Mainloop::Event event) {
		if ((event & EPOLLHUP) || (event & EPOLLRDHUP)) {
			unsubscribeNotification(name, fd);
			return;
		}
	};

	notificationLock.lock();
	if (!notificationRegistry.count(name)) {
		notificationLock.unlock();
		return -1;
	}

	Notification& notification = notificationRegistry[name];
	notificationLock.unlock();

	try {
		SubscriptionId slot = notification.createSubscriber();
		mainloop.addEventSource(slot.first, EPOLLHUP | EPOLLRDHUP, closeHandler);
		return slot.second;
	} catch (runtime::Exception& e) {
		ERROR(KSINK, e.what());
		return -1;
	}

	return -1;
}

int Service::unsubscribeNotification(const std::string& name, const int id)
{
	notificationLock.lock();

	if (!notificationRegistry.count(name)) {
		notificationLock.unlock();
		return -1;
	}

	Notification& notification = notificationRegistry[name];
	notificationLock.unlock();

	mainloop.removeEventSource(id);

	notification.removeSubscriber(id);

	return 0;
}

void Service::onMessageProcess(const std::shared_ptr<Connection>& connection)
{
	// The connection object can be destroyed in main-thread when peer is closed.
	// To make sure that the connection object is valid on that situation,
	// we should increase the reference count of the shared_ptr by capturing it as value
	auto process = [&, connection](Message& request) {
		try {
			std::shared_ptr<MethodContext> methodContext = methodRegistry.at(request.target());

			processingContext = ProcessingContext(connection);
			bool allowed = onPrivilegeCheck(processingContext.credentials, methodContext->privilege);
			onAuditTrail(processingContext.credentials, request.target(), allowed);
			if (!allowed) {
				throw runtime::Exception("Permission denied");
			}

			connection->send(methodContext->dispatcher(request));
		} catch (std::exception& e) {
			try {
				// Forward the exception to the peer
				connection->send(request.createErrorMessage(e.what()));
			} catch (std::exception& ex) {
				// The connection is abnormally closed by the peer.
				ERROR(KSINK, ex.what());
			}
		}
	};

	try {
		workqueue.submit(std::bind(process, connection->dispatch()));
	} catch (std::exception& e) {
		ERROR(KSINK, e.what());
	}
}

} // namespace rmi
