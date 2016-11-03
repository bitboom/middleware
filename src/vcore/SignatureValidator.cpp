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
 * @file        SignatureValidator.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Implementatin of tizen signature validation protocol.
 */
#include <vcore/SignatureValidator.h>
#include <vcore/BaseValidator.h>

namespace ValidationCore {

class SignatureValidator::Impl : public BaseValidator {
public:
	Impl(const SignatureFileInfo &info);
	virtual ~Impl() {};

	VCerr check(const std::string &contentPath,
				bool checkOcsp,
				bool checkReferences,
				SignatureData &outData);
	VCerr checkList(bool checkOcsp,
					const UriList &uriList,
					SignatureData &outData);
};

SignatureValidator::Impl::Impl(const SignatureFileInfo &info) :
	BaseValidator(info)
{
}

VCerr SignatureValidator::Impl::check(const std::string &contentPath,
									  bool checkOcsp,
									  bool checkReferences,
									  SignatureData &outData)
{
	VCerr result;
	result = baseCheck(contentPath, checkOcsp, checkReferences);
	result = additionalCheck(result);
	outData = m_data;
	return result;
}

VCerr SignatureValidator::Impl::checkList(
	bool checkOcsp,
	const UriList &uriList,
	SignatureData &outData)
{
	VCerr result;
	result = baseCheckList(checkOcsp, uriList);
	result = additionalCheck(result);
	outData = m_data;
	return result;
}

SignatureValidator::SignatureValidator(const SignatureFileInfo &info)
{
	using SVImpl = SignatureValidator::Impl;
	std::unique_ptr<SVImpl> impl(new(std::nothrow) SVImpl(info));
	m_pImpl = std::move(impl);
}

SignatureValidator::~SignatureValidator() {}

VCerr SignatureValidator::check(const std::string &contentPath,
								bool checkOcsp,
								bool checkReferences,
								SignatureData &outData)
{
	if (!m_pImpl)
		return E_SIG_OUT_OF_MEM;

	return m_pImpl->check(contentPath, checkOcsp, checkReferences, outData);
}

VCerr SignatureValidator::checkList(bool checkOcsp,
									const UriList &uriList,
									SignatureData &outData)
{
	if (!m_pImpl)
		return E_SIG_OUT_OF_MEM;

	return m_pImpl->checkList(checkOcsp, uriList, outData);
}

VCerr SignatureValidator::makeChainBySignature(
	bool completeWithSystemCert,
	CertificateList &certList)
{
	if (!m_pImpl)
		return E_SIG_OUT_OF_MEM;

	return m_pImpl->makeChainBySignature(completeWithSystemCert, certList);
}

std::string SignatureValidator::errorToString(VCerr code)
{
	if (!m_pImpl)
		return "out of memory. error.";

	return m_pImpl->errorToString(code);
}

} // namespace ValidationCore
