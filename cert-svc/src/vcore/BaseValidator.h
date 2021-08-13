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
 * @file        BaseValidator.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Implementation of based validation protocol.
 */
#pragma once

#include <string>
#include <list>
#include <memory>

#include <vcore/Certificate.h>
#include <vcore/SignatureData.h>
#include <vcore/SignatureFinder.h>
#include <vcore/PluginHandler.h>
#include <vcore/XmlsecAdapter.h>
#include <vcore/Error.h>

namespace ValidationCore {

using UriList = std::list<std::string>;

class BaseValidator {
public:
	explicit BaseValidator(const SignatureFileInfo &info);
	explicit BaseValidator(const std::string &packagePath);
	virtual ~BaseValidator() {}

	VCerr makeChainBySignature(bool completeWithSystemCert,
							   CertificateList &certList);
	std::string errorToString(VCerr code);

protected:
	VCerr baseCheck(const std::string &contentPath,
					bool checkOcsp,
					bool checkReferences);
	VCerr baseCheckList(bool checkOcsp, const UriList &uriList);
	VCerr additionalCheck(VCerr result);

	SignatureData m_data;
	bool m_disregarded;
	std::string m_packagePath;
	SignatureFileInfo m_fileInfo;
	SignatureFileInfoSet m_fileInfoSet;
	XmlSec::XmlSecContext m_context;

private:
	VCerr makeDataBySignature(bool completeWithSystemCert);

	VCerr parseSignature(void);
	VCerr preStep(void);
	bool checkRoleURI(void);
	bool checkProfileURI(void);
	bool checkObjectReferences(void);

	PluginHandler m_pluginHandler;
	XmlSec m_xmlSec;
};

} // namespace ValidationCore
