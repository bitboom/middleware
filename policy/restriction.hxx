/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __RESTRICTION_POLICY__
#define __RESTRICTION_POLICY__

#include "policy-context.hxx"

namespace DevicePolicyManager {

class RestrictionPolicy {
public:
	RestrictionPolicy(PolicyControlContext& ctxt);
	~RestrictionPolicy();

	RestrictionPolicy(const RestrictionPolicy& rhs);
	RestrictionPolicy(RestrictionPolicy&& rhs);

	RestrictionPolicy& operator=(const RestrictionPolicy& rhs);
	RestrictionPolicy& operator=(RestrictionPolicy&& rhs);

	int setCameraState(bool enable);
	bool getCameraState();

	int setMicrophoneState(bool enable);
	bool getMicrophoneState();

	int setClipboardState(bool enable);
	bool getClipboardState();

	int setUsbDebuggingState(bool enable);
	bool getUsbDebuggingState();

	int setUsbTetheringState(bool enable);
	bool getUsbTetheringState();

	int setExternalStorageState(bool enable);
	bool getExternalStorageState();

	int setPopImapEmailState(bool enable);
	bool getPopImapEmailState();

	int setMessagingState(const std::string& sim_id, bool enable);
	bool getMessagingState(const std::string& sim_id);

	int setBrowserState(bool enable);
	bool getBrowserState();

private:
	struct Private;
	std::unique_ptr<Private> pimpl;
};

} // namespace DevicePolicyManager

#endif /* __RESTRICTION_POLICY__ */
