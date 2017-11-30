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
 * @file        TimeConversion.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief
 */
#include <vcore/TimeConversion.h>
#include <dpl/log/log.h>

#include <memory>
#include <cstring>
#include <climits>
#include <openssl/asn1.h>

#define TIME_MAX LONG_MAX // time_t max value in arch 32bit

namespace {

const int SECONDS_PER_DAY = 24 * 60 * 60;

} // namespace anonymous

namespace ValidationCore {

int asn1TimeToTimeT(ASN1_TIME *t, time_t *res)
{
	if (res == NULL)
		return 0;

	std::unique_ptr<ASN1_TIME, decltype(&ASN1_TIME_free)> epoch(
			ASN1_TIME_set(NULL, 0),
			ASN1_TIME_free);

	if (!epoch) {
		LogError("ASN1_TIME_set() failed");
		return 0;
	}

	int day, sec;
	if (1 != ASN1_TIME_diff(&day, &sec, epoch.get(), t)) {
		LogError("ASN1_TIME_diff() failed");
		return 0;
	}

	if ((TIME_MAX - sec) / SECONDS_PER_DAY < day) {
		LogDebug("Overflow in time_t occured");
		*res = TIME_MAX;
	} else {
		*res = day * SECONDS_PER_DAY + sec;
	}

	return 1;
}

} // namespace ValidationCore
