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

#include <vist/service/vistd.hpp>

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <cstdlib>

#ifdef TIZEN
#include <vist/logger/dlog.hpp>
#endif

using namespace vist;

int main() try {
#ifdef TIZEN
	LogStream::Init(std::make_shared<Dlog>());
#endif

	Vistd::Instance().start();

	return EXIT_SUCCESS;
} catch(const Exception<ErrCode>& e) {
	ERROR(VIST) << "Failed while daemon is running." << e.what();
	return EXIT_FAILURE;
} catch(const std::exception& e) {
	ERROR(VIST) << "Failed while daemon is running." << e.what();
	return EXIT_FAILURE;
}
