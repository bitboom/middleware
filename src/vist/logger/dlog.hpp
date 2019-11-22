/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/logger.hpp>

#include <memory>

namespace vist {

class Dlog final : public LogBackend {
public:
	void info(const LogRecord& record) const noexcept override;
	void debug(const LogRecord& record) const noexcept override;
	void warn(const LogRecord& record) const noexcept override;
	void error(const LogRecord& record) const noexcept override;

private:
	/// Make default logger backend as Dlog.
	static std::shared_ptr<LogBackend> backend;
};

} // namespace vist
