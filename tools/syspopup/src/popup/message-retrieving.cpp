/*
 *
 * Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "message-retrieving.h"

MessageRetrievingPopup::MessageRetrievingPopup()
	: SyspopupInterface("IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_RETRIEVING_MESSAGES")
{
}

MessageRetrievingPopup::~MessageRetrievingPopup()
{
}

void MessageRetrievingPopup::initialize()
{
}

void MessageRetrievingPopup::contribute(const std::string &userData)
{
	createToastPopup();
}

void MessageRetrievingPopup::dispose()
{
}

SyspopupManager<MessageRetrievingPopup> messageRetrievingPopup("message-retrieving");
