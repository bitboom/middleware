/*
 *  Copyright (c) 2000 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file       journal_log_provider.h
 * @author     Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version    1.0
 */

#pragma once

#include <dpl/log/abstract_log_provider.h>

namespace VcoreDPL {
namespace Log {

class JournalLogProvider: public AbstractLogProvider {
public:
	JournalLogProvider();
	virtual ~JournalLogProvider();

	virtual void Log(AbstractLogProvider::LogLevel level,
					 const char *message,
					 const char *fileName,
					 int line,
					 const char *function) const;
};

} /* namespace Log */
} /* namespace VcoreDPL */
