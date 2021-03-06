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
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 *              Sangwan Kwon (sangwan.kwon@samsung.com)
 * @file        ReferenceValidator.h
 * @version     1.0
 * @brief       Compare signature reference list with widget package.
 */
#ifndef _VALIDATION_CORE_REFERENCEVALIDATOR_H_
#define _VALIDATION_CORE_REFERENCEVALIDATOR_H_

#include <vcore/SignatureData.h>

namespace ValidationCore {

class ReferenceValidator {
public:
	enum Result {
		NO_ERROR = 0,
		ERROR_OPENING_DIR,
		ERROR_READING_DIR,
		ERROR_UNSUPPORTED_FILE_TYPE,
		ERROR_REFERENCE_NOT_FOUND,
		ERROR_DECODING_URL,
		ERROR_OUTBOUND_LNK,
		ERROR_READING_LNK,
		ERROR_LSTAT
	};

	ReferenceValidator(const std::string &dirpath);
	virtual ~ReferenceValidator();

	ReferenceValidator(const ReferenceValidator &) = delete;
	ReferenceValidator &operator=(const ReferenceValidator &) = delete;
	ReferenceValidator(ReferenceValidator &&) = delete;
	ReferenceValidator &operator=(ReferenceValidator &&) = delete;

	Result checkReferences(const SignatureData &signatureData);
	Result checkOutbound(const std::string &linkPath, const std::string &appPath);

private:
	class Impl;
	Impl *m_impl;
};
}

#endif // _VALIDATION_CORE_REFERENCEVALIDATOR_H_
