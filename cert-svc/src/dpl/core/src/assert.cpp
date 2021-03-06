/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        assert.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of assert
 */
#include <cstdlib>
#include <sstream>
#include <stddef.h>
#include <dpl/assert.h>
#include <dpl/exception.h>
#include <dpl/log/log.h>

namespace VcoreDPL {
void AssertProc(const char *condition,
				const char *file,
				int line,
				const char *function)
{
	// Try to log failed assertion to log system
	Try {
		LogError("########################################################################");
		LogError("###                      DPL assertion failed!                       ###");
		LogError("########################################################################");
		LogError("### Condition: " << condition);
		LogError("### File: " << file);
		LogError("### Line: " << line);
		LogError("### Function: " << function);
		LogError("########################################################################");
	} catch (Exception &) {
		// Just ignore possible double errors
	}

	// Fail with c-library abort
	abort();
}
} // namespace VcoreDPL
