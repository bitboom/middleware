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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <klay/error.h>
#include <klay/exception.h>
#include <klay/xml/parser.h>
#include <klay/audit/logger.h>

#include "policy-storage.h"
#include "policy-context.hxx"

namespace {

const std::string defaultPolicyTemplate =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<manifest>\n"
"	<policy-version>0.1.0</policy-version>\n"
"	<policy-group name=\"application\">\n"
"		<policy name=\"package-installation-mode\">0</policy>\n"
"		<policy name=\"package-uninstallation-mode\">0</policy>\n"
"	</policy-group>\n"
"	<policy-group name=\"password\">\n"
"		<policy name=\"password-quality\">0</policy>\n"
"		<policy name=\"password-minimum-length\">0</policy>\n"
"		<policy name=\"password-maximum-failure-count\">0</policy>\n"
"		<policy name=\"password-expired\">0</policy>\n"
"		<policy name=\"password-minimum-complexity\">0</policy>\n"
"		<policy name=\"password-history\">0</policy>\n"
"		<policy name=\"password-recovery\">0</policy>\n"
"		<policy name=\"password-lock-delay\">0</policy>\n"
"		<policy name=\"password-inactivity-timeout\">1000</policy>\n"
"		<policy name=\"password-status\">0</policy>\n"
"		<policy name=\"password-change-timeout\">0</policy>\n"
"		<policy name=\"password-maximum-character-occurrences\">0</policy>\n"
"		<policy name=\"password-numeric-sequences-length\">0</policy>\n"
"	</policy-group>\n"
"	<policy-group name=\"security\">\n"
"		<policy name=\"internal-storage-encryption\">0</policy>\n"
"		<policy name=\"external-storage-encryption\">0</policy>\n"
"	</policy-group>\n"
"	<policy-group name=\"wifi\">\n"
"		<policy name=\"wifi-profile-change\">1</policy>\n"
"		<policy name=\"wifi-ssid-restriction\">0</policy>\n"
"	</policy-group>\n"
"	<policy-group name=\"bluetooth\">\n"
"		<policy name=\"bluetooth-pairing\">1</policy>\n"
"		<policy name=\"bluetooth-outgoing-call\">1</policy>\n"
"		<policy name=\"bluetooth-profile-a2dp\">1</policy>\n"
"		<policy name=\"bluetooth-profile-avrcp\">1</policy>\n"
"		<policy name=\"bluetooth-profile-bpp\">1</policy>\n"
"		<policy name=\"bluetooth-profile-dun\">1</policy>\n"
"		<policy name=\"bluetooth-profile-ftp\">1</policy>\n"
"		<policy name=\"bluetooth-profile-hfp\">1</policy>\n"
"		<policy name=\"bluetooth-profile-hsp\">1</policy>\n"
"		<policy name=\"bluetooth-profile-pbap\">1</policy>\n"
"		<policy name=\"bluetooth-profile-sap\">1</policy>\n"
"		<policy name=\"bluetooth-profile-spp\">1</policy>\n"
"		<policy name=\"bluetooth-desktop-connectivity\">1</policy>\n"
"		<policy name=\"bluetooth-descoverable\">1</policy>\n"
"		<policy name=\"bluetooth-limited-discoverable\">1</policy>\n"
"		<policy name=\"bluetooth-data-transfer\">1</policy>\n"
"		<policy name=\"bluetooth-uuid-restriction\">0</policy>\n"
"		<policy name=\"bluetooth-device-restriction\">0</policy>\n"
"	</policy-group>\n"
"	<policy-group name=\"restriction\">\n"
"		<policy name=\"wifi\">1</policy>\n"
"		<policy name=\"wifi-hotspot\">1</policy>\n"
"		<policy name=\"bluetooth\">1</policy>\n"
"		<policy name=\"camera\">1</policy>\n"
"		<policy name=\"microphone\">1</policy>\n"
"		<policy name=\"location\">1</policy>\n"
"		<policy name=\"external-storage\">1</policy>\n"
"		<policy name=\"messaging\">1</policy>\n"
"		<policy name=\"popimap-email\">1</policy>\n"
"		<policy name=\"browser\">1</policy>\n"
"		<policy name=\"bluetooth-tethering\">1</policy>\n"
"		<policy name=\"clipboard\">1</policy>\n"
"		<policy name=\"screen-capture\">1</policy>\n"
"		<policy name=\"usb-debugging\">1</policy>\n"
"		<policy name=\"usb-tethering\">1</policy>\n"
"	</policy-group>\n"
"</manifest>\n";

std::string StorageLocator(const std::string& base, const std::string& name, uid_t uid)
{
	return base + "/" + name + "-" + std::to_string(uid) + ".xml";
}

} // namespace

PolicyStorage::PolicyStorage(const std::string& storage, const std::string& name, uid_t uid, bool create) :
	user(uid), owner(name), location(StorageLocator(storage, name, uid)), data(nullptr)
{
	bool useDefaultPolicyTemplate = false;

	if (create) {
		struct stat st;
		if ((stat(location.c_str(), &st) == -1)) {
			if (errno == ENOENT) {
				useDefaultPolicyTemplate = true;
			} else {
				throw runtime::Exception(runtime::GetSystemErrorMessage());
			}
		}
	}

	if (useDefaultPolicyTemplate)
		data = std::unique_ptr<xml::Document>(xml::Parser::parseString(defaultPolicyTemplate));
	else
		data = std::unique_ptr<xml::Document>(xml::Parser::parseFile(location));

	xml::Node::NodeList nodes = data->evaluate("/manifest/policy-group/policy");
	xml::Node::NodeList::iterator it = nodes.begin();
	while (it != nodes.end()) {
		policyMap.emplace(it->getProp("name"), std::move(*it));
		++it;
	}

	if (useDefaultPolicyTemplate)
		flush();
}

PolicyStorage::~PolicyStorage()
{
}

int PolicyStorage::getPolicy(const std::string& name)
{
	if (policyMap.count(name) == 0) {
		throw runtime::Exception("Failed to find policy");
	}

	return policyMap.at(name).getContent();
}

void PolicyStorage::setPolicy(const std::string& name, int value)
{
	if (policyMap.count(name) == 0) {
		throw runtime::Exception("Failed to find policy");
	}

	policyMap.at(name).setContent(value);

	flush();
}

void PolicyStorage::flush()
{
	data->write(location, "UTF-8", true);
}

void PolicyStorage::remove()
{
	if (::unlink(location.c_str()) == -1)
		::unlink(location.c_str());
}
