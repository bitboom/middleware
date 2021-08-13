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

#pragma once

#include "column.hpp"
#include "database.hpp"
#include "table.hpp"

#define DECLARE_COLUMN(instance, name, type) \
	inline static vist::tsqb::Column instance = { name, type }
#define DECLARE_TABLE(instance, name, ...) \
	inline vist::tsqb::Table instance { name, __VA_ARGS__ }
#define DECLARE_DATABASE(instance, name, ...)  \
	inline vist::tsqb::Database instance { name, __VA_ARGS__ }
