/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "zone.hxx"

namespace DevicePolicyManager {

Zone::Zone(PolicyControlContext& ctx)
    : context(ctx)
{
}

Zone::~Zone()
{
}

int Zone::createZone(const std::string& name, const std::string& setupWizAppid)
{
    try {
        return context->methodCall<int>("Zone::createZone", name, setupWizAppid);
    } catch (runtime::Exception& e) {
        return -1;
    }
}

int Zone::removeZone(const std::string& name)
{
    try {
        return context->methodCall<int>("Zone::removeZone", name);
    } catch (runtime::Exception& e) {
        return -1;
    }
}

int Zone::lockZone(const std::string& name)
{
    try {
        return context->methodCall<int>("Zone::lockZone", name);
    } catch (runtime::Exception& e) {
        return -1;
    }
}

int Zone::unlockZone(const std::string& name)
{
    try {
        return context->methodCall<int>("Zone::unlockZone", name);
    } catch (runtime::Exception& e) {
        return -1;
    }
}

std::vector<std::string> Zone::getZoneList()
{
    std::vector<std::string> empty;
    try {
        return  context->methodCall<std::vector<std::string>>("Zone::getZoneList");
    } catch (runtime::Exception& e) {
        return empty;
    }
}

int Zone::getZoneState(const std::string& name)
{
    try {
        return context->methodCall<int>("Zone::getZoneState", name);
    } catch (runtime::Exception& e) {
        return -1;
    }
}

} // namespace DevicePolicyManager
