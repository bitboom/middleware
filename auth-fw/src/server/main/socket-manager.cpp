/*
 *  Copyright (c) 2000 - 2019 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Jooseong Lee <jooseong.lee@samsung.com>
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
/*
 * @file        socket-manager.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       Implementation of SocketManager.
 */

#include <set>

#include <signal.h>
#include <sys/select.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/smack.h>
#include <linux/xattr.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include <systemd/sd-daemon.h>

#include <dpl/log/log.h>
#include <dpl/assert.h>

#include <error-description.h>
#include <smack-check.h>
#include <socket-manager.h>

namespace {

const time_t SOCKET_TIMEOUT = 30;

} // namespace anonymous

namespace AuthPasswd {

struct DummyService : public GenericSocketService {
	ServiceDescriptionVector GetServiceDescription() {
		return ServiceDescriptionVector();
	}

	void Start() {}
	void Stop() {}

	void Event(const AcceptEvent &) {}
	void Event(const WriteEvent &) {}
	void Event(const ReadEvent &) {}
	void Event(const CloseEvent &) {}
};

struct SignalService : public GenericSocketService {
	int GetDescriptor() {
		LogInfo("set up");
		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGTERM);

		if (-1 == pthread_sigmask(SIG_BLOCK, &mask, NULL))
			return -1;

		return signalfd(-1, &mask, 0);
	}

	ServiceDescriptionVector GetServiceDescription() {
		return ServiceDescriptionVector();
	}

	void Start() {}
	void Stop() {}

	void Event(const AcceptEvent &) {}
	void Event(const WriteEvent &) {}
	void Event(const CloseEvent &) {}

	void Event(const ReadEvent &event) {
		LogDebug("Get signal information");

		if (sizeof(struct signalfd_siginfo) != event.rawBuffer.size()) {
			LogError("Wrong size of signalfd_siginfo struct. Expected: "
					 << sizeof(signalfd_siginfo) << " Get: "
					 << event.rawBuffer.size());
			return;
		}

		const signalfd_siginfo *siginfo = reinterpret_cast<const signalfd_siginfo *>(
											  reinterpret_cast<const void *>(event.rawBuffer.data()));

		if (siginfo->ssi_signo == SIGTERM) {
			LogInfo("Got signal: SIGTERM");
			auto socketManager = static_cast<SocketManager *>(m_serviceManager);
			if (socketManager != nullptr)
				socketManager->MainLoopStop();
			return;
		}

		LogInfo("This should not happend. Got signal: " << siginfo->ssi_signo);
	}
};

SocketManager::SocketDescription &
SocketManager::CreateDefaultReadSocketDescription(int sock, bool timeout,
		InterfaceID ifaceID /*= 0*/, GenericSocketService *service /* = NULL*/)
{
	if ((int)m_socketDescriptionVector.size() <= sock)
		m_socketDescriptionVector.resize(sock + 20);

	auto &desc = m_socketDescriptionVector[sock];
	desc.isListen = false;
	desc.isOpen = true;
	desc.interfaceID = ifaceID;
	desc.service = service;
	desc.counter = ++m_counter;

	if (timeout) {
		desc.timeout = monotonicNow() + SOCKET_TIMEOUT;

		if (false == desc.isTimeout) {
			Timeout tm;
			tm.time = desc.timeout;
			tm.sock = sock;
			m_timeoutQueue.push(tm);
		}
	}

	desc.isTimeout = timeout;
	FD_SET(sock, &m_readSet);
	m_maxDesc = sock > m_maxDesc ? sock : m_maxDesc;
	return desc;
}

SocketManager::SocketManager() :
	m_maxDesc(0),
	m_working(false),
	m_counter(0)
{
	FD_ZERO(&m_readSet);
	FD_ZERO(&m_writeSet);

	if (-1 == pipe(m_notifyMe)) {
		int err = errno;
		ThrowMsg(Exception::InitFailed, "Error in pipe: " << errnoToString(err));
	}

	LogInfo("Pipe: Read desc: " << m_notifyMe[0] << " Write desc: " << m_notifyMe[1]);
	auto &desc = CreateDefaultReadSocketDescription(m_notifyMe[0], false);
	desc.service = new DummyService;
	// std::thread bases on pthread so this should work fine
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
	// add support for TERM signal (passed from systemd)
	auto *signalService = new SignalService;
	signalService->SetSocketManager(this);
	int filefd = signalService->GetDescriptor();

	if (-1 == filefd) {
		LogError("Error in SignalService.GetDescriptor()");
		delete signalService;
	} else {
		auto &desc2 = CreateDefaultReadSocketDescription(filefd, false);
		desc2.service = signalService;
		LogInfo("SignalService mounted on " << filefd << " descriptor");
	}
}

SocketManager::~SocketManager()
{
	std::set<GenericSocketService *> serviceMap;

	// Find all services. Set is used to remove duplicates.
	// In this implementation, services are not able to react in any way.
	for (size_t i = 0; i < m_socketDescriptionVector.size(); ++i)
		if (m_socketDescriptionVector[i].isOpen)
			serviceMap.insert(m_socketDescriptionVector[i].service);

	// Time to destroy all services.
	for (auto service : serviceMap) {
		LogDebug("delete " << (void *)(service));
		service->Stop();
		delete service;
	}

	for (size_t i = 0; i < m_socketDescriptionVector.size(); ++i)
		if (m_socketDescriptionVector[i].isOpen)
			close(i);

	// All socket except one were closed. Now pipe input must be closed.
	close(m_notifyMe[1]);
}

void SocketManager::ReadyForAccept(int sock)
{
	struct sockaddr_un clientAddr;
	unsigned int clientLen = sizeof(clientAddr);
	int client = accept4(sock, (struct sockaddr *) &clientAddr, &clientLen, SOCK_NONBLOCK);

	//    LogInfo("Accept on sock: " << sock << " Socket opended: " << client);
	if (-1 == client) {
		int err = errno;
		LogDebug("Error in accept: " << errnoToString(err));
		return;
	}

	auto &desc = CreateDefaultReadSocketDescription(client, false);
	desc.interfaceID = m_socketDescriptionVector[sock].interfaceID;
	desc.service = m_socketDescriptionVector[sock].service;
	desc.useSendMsg = m_socketDescriptionVector[sock].useSendMsg;
	GenericSocketService::AcceptEvent event;
	event.connectionID.sock = client;
	event.connectionID.counter = desc.counter;
	event.interfaceID = desc.interfaceID;
	desc.service->Event(event);
}

void SocketManager::ReadyForRead(int sock)
{
	if (m_socketDescriptionVector[sock].isListen) {
		ReadyForAccept(sock);
		return;
	}

	auto &desc = m_socketDescriptionVector[sock];
	GenericSocketService::ReadEvent event;
	event.connectionID.sock = sock;
	event.connectionID.counter = m_socketDescriptionVector[sock].counter;
	event.interfaceID = desc.interfaceID;
	event.rawBuffer.resize(4096);
	desc.timeout = monotonicNow() + SOCKET_TIMEOUT;
	ssize_t size = read(sock, &event.rawBuffer[0], 4096);

	if (size == 0) {
		CloseSocket(sock);
	} else if (size >= 0) {
		event.rawBuffer.resize(size);
		desc.service->Event(event);
	} else if (size == -1) {
		int err = errno;

		switch (err) {
		case EAGAIN:
		case EINTR:
			break;

		default:
			LogDebug("Reading sock error: " << errnoToString(err));
			CloseSocket(sock);
		}
	}
}

void SocketManager::ReadyForSendMsg(int sock)
{
	auto &desc = m_socketDescriptionVector[sock];

	if (desc.sendMsgDataQueue.empty()) {
		FD_CLR(sock, &m_writeSet);
		return;
	}

	auto data = desc.sendMsgDataQueue.front();
	ssize_t result = sendmsg(sock, data.getMsghdr(), data.flags());

	if (result == -1) {
		int err = errno;

		switch (err) {
		case EAGAIN:
		case EINTR:
			break;

		case EPIPE:
		default:
			LogDebug("Error during send: " << errnoToString(err));
			CloseSocket(sock);
			break;
		}

		return;
	} else {
		desc.sendMsgDataQueue.pop();
	}

	if (desc.sendMsgDataQueue.empty())
		FD_CLR(sock, &m_writeSet);

	desc.timeout = monotonicNow() + SOCKET_TIMEOUT;
	GenericSocketService::WriteEvent event;
	event.connectionID.sock = sock;
	event.connectionID.counter = desc.counter;
	event.size = result;
	event.left = desc.sendMsgDataQueue.size();
	desc.service->Event(event);
}

void SocketManager::ReadyForWriteBuffer(int sock)
{
	auto &desc = m_socketDescriptionVector[sock];
	size_t size = desc.rawBuffer.size();
	ssize_t result = write(sock, &desc.rawBuffer[0], size);

	if (result == -1) {
		int err = errno;

		switch (err) {
		case EAGAIN:
		case EINTR:
			// select will trigger write once again, nothing to do
			break;

		case EPIPE:
		default:
			LogDebug("Error during write: " << errnoToString(err));
			CloseSocket(sock);
			break;
		}

		return; // We do not want to propagate error to next layer
	}

	desc.rawBuffer.erase(desc.rawBuffer.begin(), desc.rawBuffer.begin() + result);
	desc.timeout = monotonicNow() + SOCKET_TIMEOUT;

	if (desc.rawBuffer.empty())
		FD_CLR(sock, &m_writeSet);

	GenericSocketService::WriteEvent event;
	event.connectionID.sock = sock;
	event.connectionID.counter = desc.counter;
	event.size = result;
	event.left = desc.rawBuffer.size();
	desc.service->Event(event);
}

void SocketManager::ReadyForWrite(int sock)
{
	m_socketDescriptionVector[sock].useSendMsg ?
	ReadyForSendMsg(sock) : ReadyForWriteBuffer(sock);
}

void SocketManager::MainLoop()
{
	// remove evironment values passed by systemd
	// uncomment it after removing old authentication password code
	// sd_listen_fds(1);
	// Daemon is ready to work.
	sd_notify(0, "READY=1");
	m_working = true;

	while (m_working) {
		fd_set readSet = m_readSet;
		fd_set writeSet = m_writeSet;
		timeval localTempTimeout;
		timeval *ptrTimeout = &localTempTimeout;

		// I need to extract timeout from priority_queue.
		// Timeout in priority_queue may be deprecated.
		// I need to find some actual one.
		while (!m_timeoutQueue.empty()) {
			auto &top = m_timeoutQueue.top();
			auto &desc = m_socketDescriptionVector[top.sock];

			if (top.time == desc.timeout) {
				// This timeout matches timeout from socket.
				// It can be used.
				break;
			} else {
				// This socket was used after timeout in priority queue was set up.
				// We need to update timeout and find some useable one.
				Timeout tm = { desc.timeout , top.sock};
				m_timeoutQueue.pop();
				m_timeoutQueue.push(tm);
			}
		}

		if (m_timeoutQueue.empty()) {
			LogDebug("No usaable timeout found.");
	  		ptrTimeout->tv_sec = SOCKET_TIMEOUT;
			ptrTimeout->tv_usec = 0;
		} else {
			time_t currentTime = monotonicNow();
			auto &pqTimeout = m_timeoutQueue.top();
			// 0 means that select won't block and socket will be closed ;-)
			ptrTimeout->tv_sec =
				currentTime < pqTimeout.time ? pqTimeout.time - currentTime : 0;
			ptrTimeout->tv_usec = 0;
			//            LogDebug("Set up timeout: " << (int)ptrTimeout->tv_sec
			//                << " seconds. Socket: " << pqTimeout.sock);
		}

		int ret = select(m_maxDesc + 1, &readSet, &writeSet, NULL, ptrTimeout);

		if (0 == ret) { // timeout
			if (!m_timeoutQueue.empty()) {
				Timeout pqTimeout = m_timeoutQueue.top();
				m_timeoutQueue.pop();
				auto &desc = m_socketDescriptionVector[pqTimeout.sock];

				if (!desc.isTimeout || !desc.isOpen) {
					// Connection was closed. Timeout is useless...
					desc.isTimeout = false;
					continue;
				}

				if (pqTimeout.time < desc.timeout) {
					// Is it possible?
					// This socket was used after timeout. We need to update timeout.
					pqTimeout.time = desc.timeout;
					m_timeoutQueue.push(pqTimeout);
					continue;
				}

				// timeout from m_timeoutQueue matches with socket.timeout
				// and connection is open. Time to close it!
				// Putting new timeout in queue here is pointless.
				desc.isTimeout = false;
				CloseSocket(pqTimeout.sock);
			}
			// All done. Now we should process next select ;-)
			m_working = false;
			continue;
		}

		if (-1 == ret) {
			switch (errno) {
			case EINTR:
				LogDebug("EINTR in select");
				break;

			default:
				int err = errno;
				LogError("Error in select: " << errnoToString(err));
				return;
			}

			continue;
		}

		for (int i = 0; i < m_maxDesc + 1 && ret; ++i) {
			if (FD_ISSET(i, &readSet)) {
				ReadyForRead(i);
				--ret;
			}

			if (FD_ISSET(i, &writeSet)) {
				ReadyForWrite(i);
				--ret;
			}
		}

		ProcessQueue();
	}
}

void SocketManager::MainLoopStop()
{
	m_working = false;
	NotifyMe();
}

int SocketManager::GetSocketFromSystemD(
	const GenericSocketService::ServiceDescription &desc)
{
	int fd;
	// TODO optimalization - do it once in object constructor
	//                       and remember all information path->sockfd
	int n = sd_listen_fds(0);
	LogInfo("sd_listen_fds returns: " << n);

	if (n < 0) {
		LogError("Error in sd_listend_fds");
		ThrowMsg(Exception::InitFailed, "Error in sd_listend_fds");
	}

	for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + n; ++fd) {
		if (0 < sd_is_socket_unix(fd, SOCK_STREAM, 1,
								  desc.serviceHandlerPath.c_str(), 0)) {
			LogInfo("Useable socket " << desc.serviceHandlerPath <<
					" was passed by SystemD under descriptor " << fd);
			return fd;
		}
	}

	LogInfo("No useable sockets were passed by systemd.");
	return -1;
}

int SocketManager::CreateDomainSocketHelp(
	const GenericSocketService::ServiceDescription &desc)
{
	int sockfd;
	GenericSocketService::ServiceHandlerPath::size_type maxlen =
		sizeof(static_cast<sockaddr_un *>(nullptr)->sun_path) /
		sizeof(GenericSocketService::ServiceHandlerPath::value_type);

	if (desc.serviceHandlerPath.size() >= maxlen) {
		LogError("Service handler path too long: " << desc.serviceHandlerPath.size());
		ThrowMsg(Exception::InitFailed,
				 "Service handler path too long: " << desc.serviceHandlerPath.size());
	}

	if (-1 == (sockfd = socket(AF_UNIX, SOCK_STREAM, 0))) {
		int err = errno;
		LogError("Error in socket: " << errnoToString(err));
		ThrowMsg(Exception::InitFailed, "Error in socket: " << errnoToString(err));
	}

	if (smack_check()) {
		LogInfo("Set up smack label: " << desc.smackLabel);

		if (0 != smack_set_label_for_file(sockfd, XATTR_NAME_SMACKIPIN, desc.smackLabel.c_str())) {
			LogError("Error in smack_set_label_for_file");
			ThrowMsg(Exception::InitFailed, "Error in smack_set_label_for_file");
		}
	} else {
		LogInfo("No smack on platform. Socket won't be securied with smack label!");
	}

	int flags;

	if (-1 == (flags = fcntl(sockfd, F_GETFL, 0)))
		flags = 0;

	if (-1 == fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) {
		int err = errno;
		close(sockfd);
		LogError("Error in fcntl: " << errnoToString(err));
		ThrowMsg(Exception::InitFailed, "Error in fcntl: " << errnoToString(err));
	}

	sockaddr_un serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sun_family = AF_UNIX;
	strncpy(serverAddress.sun_path, desc.serviceHandlerPath.c_str(),
			sizeof(serverAddress.sun_path) - 1);
	unlink(serverAddress.sun_path);
	mode_t originalUmask;
	originalUmask = umask(0);

	if (-1 == bind(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress))) {
		int err = errno;
		close(sockfd);
		LogError("Error in bind: " << errnoToString(err));
		ThrowMsg(Exception::InitFailed, "Error in bind: " << errnoToString(err));
	}

	umask(originalUmask);

	if (-1 == listen(sockfd, 5)) {
		int err = errno;
		close(sockfd);
		LogError("Error in listen: " << errnoToString(err));
		ThrowMsg(Exception::InitFailed, "Error in listen: " << errnoToString(err));
	}

	return sockfd;
}

void SocketManager::CreateDomainSocket(
	GenericSocketService *service,
	const GenericSocketService::ServiceDescription &desc)
{
	int sockfd = GetSocketFromSystemD(desc);

	if (-1 == sockfd)
		sockfd = CreateDomainSocketHelp(desc);

	auto &description = CreateDefaultReadSocketDescription(sockfd, false);
	description.isListen = true;
	description.interfaceID = desc.interfaceID;
	description.useSendMsg = desc.useSendMsg;
	description.service = service;
	LogDebug("Listen on socket: " << sockfd <<
			 " Handler: " << desc.serviceHandlerPath.c_str());
}

void SocketManager::RegisterSocketService(GenericSocketService *service)
{
	service->SetSocketManager(this);
	auto serviceVector = service->GetServiceDescription();
	Try {
		for (auto iter = serviceVector.begin(); iter != serviceVector.end(); ++iter) {
			switch (iter->type) {
			case GenericSocketService::ServiceDescription::SOCKET_SERVICE:
				CreateDomainSocket(service, *iter);
				break;

			case GenericSocketService::ServiceDescription::FILE_DESC_SERVICE:
				CreateDefaultReadSocketDescription(iter->fileDesc, false, iter->interfaceID, service);
				break;

			default:
				ThrowMsg(Exception::InitFailed, "Wrong service type in service description: " << iter->type);
				break;
			}
		}
	} Catch(Exception::Base) {
		for (int i = 0; i < (int)m_socketDescriptionVector.size(); ++i) {
			auto &desc = m_socketDescriptionVector[i];

			if (desc.service == service && desc.isOpen) {
				close(i);
				desc.isOpen = false;
			}
		}

		ReThrow(Exception::Base);
	}
}

void SocketManager::Close(ConnectionID connectionID)
{
	do {
		std::lock_guard<std::mutex> ulock(m_eventQueueMutex);
		m_closeQueue.push(connectionID);
	} while (false);

	NotifyMe();
}

void SocketManager::Write(ConnectionID connectionID, const RawBuffer &rawBuffer)
{
	WriteBuffer buffer;
	buffer.connectionID = connectionID;
	buffer.rawBuffer = rawBuffer;

	do {
		std::lock_guard<std::mutex> ulock(m_eventQueueMutex);
		m_writeBufferQueue.push(buffer);
	} while (false);

	NotifyMe();
}

void SocketManager::Write(ConnectionID connectionID, const SendMsgData &sendMsgData)
{
	WriteData data;
	data.connectionID = connectionID;
	data.sendMsgData = sendMsgData;

	do {
		std::lock_guard<std::mutex> ulock(m_eventQueueMutex);
		m_writeDataQueue.push(data);
	} while (false);

	NotifyMe();
}

void SocketManager::NotifyMe()
{
	TEMP_FAILURE_RETRY(write(m_notifyMe[1], "You have message ;-)", 1));
}

void SocketManager::ProcessQueue()
{
	do {
		std::lock_guard<std::mutex> ulock(m_eventQueueMutex);

		WriteBuffer buffer;
		WriteData data;

		while (!m_writeBufferQueue.empty()) {
			buffer = m_writeBufferQueue.front();
			m_writeBufferQueue.pop();
			auto &desc = m_socketDescriptionVector[buffer.connectionID.sock];

			if (!desc.isOpen) {
				LogDebug("Received packet for write but connection is closed. Packet ignored!");
				continue;
			}

			if (desc.counter != buffer.connectionID.counter) {
				LogDebug("Received packet for write but counter is broken. Packet ignored!");
				continue;
			}

			if (desc.useSendMsg) {
				LogError("Some service tried to push rawdata to socket that usees sendmsg!");
				continue;
			}

			std::copy(
				buffer.rawBuffer.begin(),
				buffer.rawBuffer.end(),
				std::back_inserter(desc.rawBuffer));
			FD_SET(buffer.connectionID.sock, &m_writeSet);
		}

		while (!m_writeDataQueue.empty()) {
			data = m_writeDataQueue.front();
			m_writeDataQueue.pop();
			auto &desc = m_socketDescriptionVector[data.connectionID.sock];

			if (!desc.isOpen) {
				LogDebug("Received packet for sendmsg but connection is closed. Packet ignored!");
				continue;
			}

			if (desc.counter != data.connectionID.counter) {
				LogDebug("Received packet for write but counter is broken. Packet ignored!");
				continue;
			}

			if (!desc.useSendMsg) {
				LogError("Some service tries to push SendMsgData to socket that uses write!");
				continue;
			}

			desc.sendMsgDataQueue.push(data.sendMsgData);
			FD_SET(data.connectionID.sock, &m_writeSet);
		}
	} while (false);

	while (true) {
		ConnectionID connection;
		do {
			std::lock_guard<std::mutex> ulock(m_eventQueueMutex);

			if (m_closeQueue.empty())
				return;

			connection = m_closeQueue.front();
			m_closeQueue.pop();
		} while (false);

		if (!m_socketDescriptionVector[connection.sock].isOpen)
			continue;

		if (connection.counter != m_socketDescriptionVector[connection.sock].counter)
			continue;

		CloseSocket(connection.sock);
	}
}

void SocketManager::CloseSocket(int sock)
{
	// LogInfo("Closing socket: " << sock);
	auto &desc = m_socketDescriptionVector[sock];

	if (!(desc.isOpen)) {
		// This may happend when some information was waiting for write to the
		// socket and in the same time socket was closed by the client.
		LogError("Socket " << sock << " is not open. Nothing to do!");
		return;
	}

	GenericSocketService::CloseEvent event;
	event.connectionID.sock = sock;
	event.connectionID.counter = desc.counter;
	auto service = desc.service;
	desc.isOpen = false;
	desc.service = NULL;
	desc.interfaceID = -1;
	desc.rawBuffer.clear();

	while (!desc.sendMsgDataQueue.empty())
		desc.sendMsgDataQueue.pop();

	if (service)
		service->Event(event);
	else
		LogError("Critical! Service is NULL! This should never happen!");

	TEMP_FAILURE_RETRY(close(sock));
	FD_CLR(sock, &m_readSet);
	FD_CLR(sock, &m_writeSet);
}

time_t SocketManager::monotonicNow() {
	struct timespec now;
	if (clock_gettime(CLOCK_MONOTONIC_RAW, &now) == -1) {
		int err = errno;
		LogError("Can't access monothonic clock, error: " <<  errnoToString(err));
		return 0;
	}
	return now.tv_sec;
}

} // namespace AuthPasswd
