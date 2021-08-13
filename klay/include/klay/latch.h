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

#ifndef __RUNTIME_LATCH_H__
#define __RUNTIME_LATCH_H__

#include <mutex>
#include <condition_variable>

#include <klay/klay.h>

namespace klay {

class KLAY_EXPORT Latch {
public:
	Latch() :
		count(0)
	{
	}

	void set()
	{
		std::unique_lock<std::mutex> lock(mutex);
		++count;
		condition.notify_one();
	}

	void wait()
	{
		waitForN(1);
	}

	bool wait(unsigned int timeout)
	{
		return waitForN(1, timeout);
	}

	void waitForN(unsigned int n)
	{
		std::unique_lock<std::mutex> lock(mutex);
		condition.wait(lock, [this, n] { return count >= n; });
		count -= n;
	}

	bool waitForN(unsigned int n, unsigned int timeout)
	{
		std::unique_lock<std::mutex> lock(mutex);
		if (!condition.wait_for(lock, std::chrono::milliseconds(timeout),
								[this, n] { return count >= n; })) {
			return false;
		}

		count -= n;
		return true;
	}
private:
	std::mutex mutex;
	std::condition_variable condition;
	unsigned int count;
};

} // namespace klay

namespace runtime = klay;

#endif // __RUNTIME_LATCH_H__
