/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        assert.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of assert
 */
#ifndef CCHECKER_ASSERT_H
#define CCHECKER_ASSERT_H

#include <cchecker/dpl/noreturn.h>

namespace CCHECKER {
// Assertion handler procedure
// Do not call directly
// Always use Assert macro
CCHECKER_NORETURN void AssertProc(const char *condition,
								  const char *file,
								  int line,
								  const char *function);
} // namespace CCHECKER

#define Assert(Condition) do { if (!(Condition)) { CCHECKER::AssertProc(#Condition, \
				__FILE__, \
				__LINE__, \
				__FUNCTION__); \
	} } while (0)

#endif // CCHECKER_ASSERT_H
