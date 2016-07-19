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

#ifndef __XML_KEEPBLANKS_H__
#define __XML_KEEPBLANKS_H__

#include <libxml/globals.h>

namespace xml {

class KeepBlanks {
public:
	KeepBlanks(bool value);
	~KeepBlanks();

	static const bool Default = true;

private:
	int preservedKeepBlanksDefault;
	int preservedIndentTreeOutput;
};

} // namespace xml
#endif //__XML_KEEPBLANKS_H__
