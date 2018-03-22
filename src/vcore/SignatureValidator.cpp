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

#include <dpl/log/log.h>

#include <utility>

namespace ValidationCore {

class SignatureValidator::Impl : public BaseValidator {
public:
	explicit Impl(const SignatureFileInfo &info);
	explicit Impl(const std::string &packagePath);
	virtual ~Impl() {}

	VCerr check(const std::string &contentPath,
				bool checkOcsp,
				bool checkReferences,
				SignatureData &outData);
	VCerr checkList(bool checkOcsp,
					const UriList &uriList,
					SignatureData &outData);

	VCerr checkAll(bool checkOcsp,
				   bool checkReferences,
				   SignatureDataMap &sigDataMap);
	VCerr checkListAll(bool checkOcsp,
					   const UriList &uriList,
					   SignatureDataMap &sigDataMap);
};

SignatureValidator::Impl::Impl(const SignatureFileInfo &info) :
	BaseValidator(info)
{
}

SignatureValidator::Impl::Impl(const std::string &packagePath) :
	BaseValidator(packagePath)
{
	m_context.isProxyMode = true;

	SignatureFinder signatureFinder(m_packagePath);
	if (SignatureFinder::NO_ERROR != signatureFinder.find(m_fileInfoSet))
		LogError("Failed to find signature files.");
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

VCerr SignatureValidator::Impl::checkList(bool checkOcsp,
										  const UriList &uriList,
										  SignatureData &outData)
{
	VCerr result;
	result = baseCheckList(checkOcsp, uriList);
	result = additionalCheck(result);
	outData = m_data;
	return result;
}

VCerr SignatureValidator::Impl::checkAll(bool checkOcsp,
										 bool checkReferences,
										 SignatureDataMap &sigDataMap)
{
	if (m_fileInfoSet.size() < 2)
		return E_SIG_UNKNOWN; // TODO(sangwan.kwon) Add error code (INVALID SIZE)

	VCerr result = E_SIG_UNKNOWN;
	for (const auto &sig : m_fileInfoSet) {
		m_fileInfo = sig;
		m_disregarded = false;

		result = baseCheck(m_packagePath, checkOcsp, checkReferences);
		result = additionalCheck(result);
		if (result != E_SIG_NONE) {
			LogError("Failed to check on > " << m_fileInfo.getFileName());
			break;
		}
		sigDataMap.insert(std::make_pair(m_data.getSignatureNumber(), m_data));
		LogDebug("Check done signature > " << m_data.getSignatureNumber());
		for (const auto &certPtr : m_data.getCertList())
			LogDebug(certPtr->getBase64());
	}

	return result;
}

VCerr SignatureValidator::Impl::checkListAll(bool checkOcsp,
											 const UriList &uriList,
											 SignatureDataMap &sigDataMap)
{
	if (m_fileInfoSet.size() < 2)
		return E_SIG_UNKNOWN; // TODO(sangwan.kwon) Add error code (INVALID SIZE)

	VCerr result = E_SIG_UNKNOWN;
	for (const auto &sig : m_fileInfoSet) {
		m_fileInfo = sig;
		m_disregarded = false;

		result = baseCheckList(checkOcsp, uriList);
		result = additionalCheck(result);
		if (result != E_SIG_NONE) {
			LogError("Failed to check on > " << m_fileInfo.getFileName());
			break;
		}
		sigDataMap.insert(std::make_pair(m_data.getSignatureNumber(), m_data));
		LogDebug("CheckList done signature > " << m_data.getSignatureNumber());
		for (const auto &certPtr : m_data.getCertList())
			LogDebug(certPtr->getBase64());
	}

	return result;
}

SignatureValidator::SignatureValidator(const SignatureFileInfo &info)
{
	using SVImpl = SignatureValidator::Impl;
	std::unique_ptr<SVImpl> impl(new(std::nothrow) SVImpl(info));
	m_pImpl = std::move(impl);
}

SignatureValidator::SignatureValidator(const std::string &packagePath)
{
	using SVImpl = SignatureValidator::Impl;
	std::unique_ptr<SVImpl> impl(new(std::nothrow) SVImpl(packagePath));
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

VCerr SignatureValidator::checkAll(bool checkOcsp,
								   bool checkReferences,
								   SignatureDataMap &sigDataMap)
{
	if (!m_pImpl)
		return E_SIG_OUT_OF_MEM;

	return m_pImpl->checkAll(checkOcsp, checkReferences, sigDataMap);
}

VCerr SignatureValidator::checkListAll(bool checkOcsp,
									   const UriList &uriList,
									   SignatureDataMap &sigDataMap)
{
	if (!m_pImpl)
		return E_SIG_OUT_OF_MEM;

	return m_pImpl->checkListAll(checkOcsp, uriList, sigDataMap);
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
