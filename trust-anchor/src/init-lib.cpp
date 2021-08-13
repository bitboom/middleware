/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        init-lib.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Init global configuration for library
 */
#include <klay/audit/dlog-sink.h>

#include <memory>

#include "logger.hxx"

namespace tanchor {
namespace {

class InitLib {
public:
	InitLib()
	{
#ifdef TIZEN_TEST_GCOV
		::setenv("GCOV_PREFIX", "/tmp", 1);
#endif

		this->m_sink.reset(new audit::DlogLogSink("TRUST_ANCHOR"));
	};
	~InitLib() = default;

	std::unique_ptr<audit::DlogLogSink> m_sink = nullptr;
};

static std::unique_ptr<InitLib> init(new(std::nothrow)(InitLib));

} // namespace anonymous

audit::LogSink *SINK = dynamic_cast<audit::LogSink*>((init->m_sink).get());

} // namespace tanchor
