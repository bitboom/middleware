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
 * @file        availability.h
 * @author      Jihoon Chung (jihoon.chung@samsung.com)
 * @version     1.0
 */
#ifndef DPL_AVAILABILITY_H
#define DPL_AVAILABILITY_H

#define DPL_DEPRECATED __attribute__((deprecated))
#define DPL_DEPRECATED_WITH_MESSAGE(msg) __attribute__((deprecated(msg)))

#define DPL_UNUSED __attribute__((unused))
#define DPL_UNUSED_PARAM(variable) (void)variable

#endif // DPL_AVAILABILITY_H
