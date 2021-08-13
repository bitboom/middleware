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

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <klay/error.h>
#include <klay/process.h>
#include <klay/exception.h>

namespace klay {

Process::Process(const std::string& prog) :
	status(-1), pid(-1), program(prog)
{
	args.push_back(prog);
}

Process::Process(const std::string& prog, const std::vector<std::string>& args) :
	status(-1), pid(-1), program(prog), args(args)
{
}

Process::~Process()
{
}

int Process::execute()
{
	pid = ::fork();
	if (pid == -1) {
		return -1;
	}

	if (pid == 0) {
		const char** argv = new const char *[args.size() + 1];

		int i = 0;
		for (std::string & arg : args) {
			argv[i++] = arg.c_str();
		}
		argv[i] = NULL;

		::execv(program.c_str(), const_cast<char* const*>(argv));
		std::quick_exit(EXIT_FAILURE);
	}

	return pid;
}

int Process::waitForFinished()
{
	while (::waitpid(pid, &status, 0) == -1) {
		if (errno != EINTR) {
			return -1;
		}
	}

	return status;
}

bool Process::isRunning() const
{
	if (::kill(pid, 0) == 0) {
		return true;
	}

	return false;
}

void Process::kill()
{
	if (::kill(pid, SIGKILL) == -1) {
		throw klay::Exception(klay::GetSystemErrorMessage());
	}
}

void Process::terminate()
{
	if (::kill(pid, SIGINT) == -1) {
		throw klay::Exception(klay::GetSystemErrorMessage());
	}
}

} // namespace klay
