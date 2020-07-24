/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include "util.hpp"

#include <vist/logger.hpp>

namespace vist {
namespace table {

QueryData exception_guard(const std::function<QueryData()>& func) try
{
	return func();
} catch (const vist::Exception<ErrCode>& e)
{
	ERROR(VIST) << "Failed while excuting query: " << e.what();
	Row r;
	return { r };
} catch (...)
{
	ERROR(VIST) << "Failed to query with unknown exception.";
	Row r;
	return { r };
}

QueryData success()
{
	Row r;
	r["status"] = "success";
	return QueryData { r };
}

} // namespace table
} // namespace vist
