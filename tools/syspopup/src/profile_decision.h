/*
 * Tizen DPM Syspopup
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __DPM_PROFILE_DECISION_H__
#define __DPM_PROFILE_DECISION_H__

extern int _tizen_whether_wearable;
extern int _get_tizen_profile();
#define _TIZEN_PROFILE_WEARABLE ((__builtin_expect(_tizen_whether_wearable != -1, 1)) ? \
                               _tizen_whether_wearable : _get_tizen_profile())

#endif /* __DPM_PROFILE_DECISION_H__ */
