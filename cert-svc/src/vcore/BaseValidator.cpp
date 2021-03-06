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
 * @file        BaseValidator.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Implementation of based validation protocol.
 */
#include <vcore/BaseValidator.h>

#include <memory>
#include <string>
#include <utility>

#include <dpl/log/log.h>

#include <vcore/CertificateCollection.h>
#include <vcore/Certificate.h>
#include <vcore/ReferenceValidator.h>
#include <vcore/ValidatorFactories.h>
#include <vcore/SignatureReader.h>
#include <vcore/SignatureFinder.h>
#include <vcore/Ocsp.h>

using namespace ValidationCore::CertStoreId;

namespace {

const std::string TOKEN_PREFIX          = "http://www.w3.org/ns/widgets-digsig#";
const std::string TOKEN_ROLE_AUTHOR_URI = TOKEN_PREFIX + "role-author";
const std::string TOKEN_ROLE_DIST_URI   = TOKEN_PREFIX + "role-distributor";
const std::string TOKEN_PROFILE_URI     = TOKEN_PREFIX + "profile";

enum class CertTimeStatus : int {
	VALID,
	NOT_YET,
	EXPIRED
};

inline time_t _getMidTime(time_t lower, time_t upper)
{
	return (lower >> 1) + (upper >> 1);
}

inline CertTimeStatus _timeValidation(time_t lower, time_t upper, time_t current)
{
	if (current < lower)
		return CertTimeStatus::NOT_YET;
	else if (current > upper)
		return CertTimeStatus::EXPIRED;
	else
		return CertTimeStatus::VALID;
}

inline bool _isTimeStrict(const Set &stores)
{
	return (stores.contains(TIZEN_TEST) || stores.contains(TIZEN_VERIFY))
		   ? true : false;
}

} // namespace anonymous

namespace ValidationCore {

BaseValidator::BaseValidator(const SignatureFileInfo &info) :
	m_disregarded(false),
	m_fileInfo(info)
{
}

BaseValidator::BaseValidator(const std::string &packagePath) :
	m_disregarded(false),
	m_packagePath(packagePath)
{
}

bool BaseValidator::checkRoleURI(void)
{
	std::string roleURI = m_data.getRoleURI();

	if (roleURI.empty()) {
		LogWarning("URI attribute in Role tag couldn't be empty.");
		return false;
	}

	if (roleURI != TOKEN_ROLE_AUTHOR_URI && m_data.isAuthorSignature()) {
		LogWarning("URI attribute in Role tag does not "
				   "match with signature filename.");
		return false;
	}

	if (roleURI != TOKEN_ROLE_DIST_URI && !m_data.isAuthorSignature()) {
		LogWarning("URI attribute in Role tag does not "
				   "match with signature filename.");
		return false;
	}

	return true;
}

bool BaseValidator::checkProfileURI(void)
{
	if (TOKEN_PROFILE_URI != m_data.getProfileURI()) {
		LogWarning("Profile tag contains unsupported value "
				   "in URI attribute " << m_data.getProfileURI());
		return false;
	}

	return true;
}

bool BaseValidator::checkObjectReferences(void)
{
	for (const auto &object : m_data.getObjectList()) {
		if (!m_data.containObjectReference(object)) {
			LogWarning("Signature does not contain reference for object " << object);
			return false;
		}
	}

	return true;
}

VCerr BaseValidator::additionalCheck(VCerr result)
{
	try {
		if (m_pluginHandler.fail()) {
			LogInfo("No validator plugin found. Skip additional check.");
			return result;
		}

		return m_pluginHandler.step(result, m_data);
	} catch (...) {
		LogError("Exception in additional check by plugin.");
		return E_SIG_PLUGIN;
	}
}

VCerr BaseValidator::parseSignature(void)
{
	try {
		SignatureReader xml;
		xml.initialize(m_data, SIGNATURE_SCHEMA_PATH);
		xml.read(m_data);
	} catch (ParserSchemaException::CertificateLoaderError &e) {
		LogError("Certificate loader error: " << e.DumpToString());
		return E_SIG_INVALID_CERT;
	} catch (...) {
		LogError("Failed to parse signature file by signature reader.");
		return E_SIG_INVALID_FORMAT;
	}

	return E_SIG_NONE;
}

/*
 *  Make SignatureData by parsing signature file.
 *  and get certificate chain with attached certificate in signature
 */
VCerr BaseValidator::makeDataBySignature(bool completeWithSystemCert)
{
	LogDebug("Start to make chain about > " << m_fileInfo.getFileName());
	m_data = SignatureData(m_fileInfo.getFileName(), m_fileInfo.getFileNumber());

	if (parseSignature()) {
		LogError("Failed to parse signature.");
		return E_SIG_INVALID_FORMAT;
	}

	if (!checkRoleURI() || !checkProfileURI())
		return E_SIG_INVALID_FORMAT;

	try {
		CertificateCollection collection;
		// Load Certificates and make chain.
		collection.load(m_data.getCertList());

		if (!collection.sort() || collection.empty()) {
			LogError("Certificates do not form valid chain.");
			return E_SIG_INVALID_CHAIN;
		}

		// Add root certificate to chain.
		if (completeWithSystemCert && !collection.completeCertificateChain()) {
			if (m_data.isAuthorSignature() || m_data.getSignatureNumber() == 1) {
				LogError("Failed to complete cert chain with system cert");
				return E_SIG_INVALID_CHAIN;
			} else {
				LogDebug("Distributor N's certificate has got "
						 "unrecognized root CA certificate.");
				m_disregarded = true;
			}
		}

		m_data.setSortedCertificateList(collection.getChain());
		LogDebug("Finish making chain successfully.");
	} catch (const CertificateCollection::Exception::Base &e) {
		LogError("CertificateCollection exception : " << e.DumpToString());
		return E_SIG_INVALID_CHAIN;
	} catch (const std::exception &e) {
		LogError("std exception occured : " << e.what());
		return E_SIG_UNKNOWN;
	} catch (...) {
		LogError("Unknown exception in BaseValidator::makeChainBySignature");
		return E_SIG_UNKNOWN;
	}

	return E_SIG_NONE;
}

VCerr BaseValidator::preStep(void)
{
	// Make chain process.
	VCerr result = makeDataBySignature(true);

	if (result != E_SIG_NONE)
		return result;

	for (const auto &certptr : m_data.getCertList()) {
		auto storeIdSet = createCertificateIdentifier().find(certptr);
		if (!storeIdSet.contains(TIZEN_REVOKED))
			continue;

		LogInfo("Revoked certificate: " << certptr->getOneLine());
		return E_SIG_REVOKED;
	}

	// Get Identifier from fingerprint original, extention file.
	LogDebug("Start to check certificate domain.");
	auto certificatePtr = m_data.getCertList().back();
	auto storeIdSet = createCertificateIdentifier().find(certificatePtr);

	// Check root CA certificate has proper domain.
	LogDebug("root certificate from " << storeIdSet.typeToString() << " domain");
	if (m_data.isAuthorSignature()) {
		if (!storeIdSet.contains(TIZEN_DEVELOPER)) {
			LogError("author-signature.xml's root certificate "
					 "isn't in tizen developer domain.");
			return E_SIG_INVALID_CHAIN;
		}
	} else {
		if (storeIdSet.contains(TIZEN_DEVELOPER)) {
			LogError("distributor signautre root certificate "
					 "shouldn't be in tizen developer domain.");
			return E_SIG_INVALID_CHAIN;
		}

		if (m_data.getSignatureNumber() == 1 && !storeIdSet.isContainsVis()) {
			LogError("signature1.xml has got unrecognized root CA certificate.");
			return E_SIG_INVALID_CHAIN;
		} else if (!storeIdSet.isContainsVis()) {
			LogDebug("signatureN.xml has got unrecognized root CA certificate.");
			m_disregarded = true;
		}
	}

	m_data.setStorageType(storeIdSet);
	LogDebug("Finish checking certificate domain.");
	/*
	 * We add only Root CA certificate because the rest
	 * of certificates are present in signature files ;-)
	 */
	m_context.signatureFile = m_data.getSignatureFileName();
	m_context.certificatePtr = m_data.getCertList().back();
	/* certificate time check */
	time_t lower = m_data.getEndEntityCertificatePtr()->getNotBefore();
	time_t upper = m_data.getEndEntityCertificatePtr()->getNotAfter();
	time_t current = time(NULL);
	CertTimeStatus status = _timeValidation(lower, upper, current);

	if (status != CertTimeStatus::VALID) {
		LogDebug("Certificate's time is invalid.");

		if (_isTimeStrict(storeIdSet))
			return status == CertTimeStatus::EXPIRED
				   ? E_SIG_CERT_EXPIRED : E_SIG_CERT_NOT_YET;

		time_t mid = _getMidTime(lower, upper);
		LogInfo("Use middle notBeforeTime and notAfterTime."
				" lower: " << lower
				<< " upper: " << upper
				<< " mid: " << mid
				<< " current: " << current);
		m_context.validationTime = mid;
	}

	return E_SIG_NONE;
}

VCerr BaseValidator::baseCheck(const std::string &contentPath,
							   bool checkOcsp,
							   bool checkReferences)
{
	try {
		// Make certificate chain, check certificate info
		VCerr result = preStep();

		if (result != E_SIG_NONE)
			return result;

		// Since disregard case, uncheck root certs in signatureN.xml
		if (!m_data.isAuthorSignature() && m_data.getSignatureNumber() != 1)
			m_context.allowBrokenChain = true;

		// XmlSec validate
		m_xmlSec.validate(m_context);
		// Check reference of 'Object' tag - OID
		m_data.setReference(m_context.referenceSet);

		if (!checkObjectReferences()) {
			LogWarning("Failed to check Object References");
			return E_SIG_INVALID_REF;
		}

		// Check reference's existence
		if (checkReferences) {
			ReferenceValidator fileValidator(contentPath);

			if (ReferenceValidator::NO_ERROR != fileValidator.checkReferences(m_data)) {
				LogWarning("Invalid package - file references broken");
				return E_SIG_INVALID_REF;
			}
		}

		// Check OCSP
		if (checkOcsp && Ocsp::check(m_data) == Ocsp::Result::REVOKED) {
			LogError("Certificate is Revoked by OCSP server.");
			return E_SIG_REVOKED;
		}

		LogDebug("Signature validation check done successfully ");
	} catch (const CertificateCollection::Exception::Base &e) {
		LogError("CertificateCollection exception : " << e.DumpToString());
		return E_SIG_INVALID_CHAIN;
	} catch (const XmlSec::Exception::InternalError &e) {
		LogError("XmlSec internal error : " << e.DumpToString());
		return E_SIG_INVALID_FORMAT;
	} catch (const XmlSec::Exception::InvalidFormat &e) {
		LogError("XmlSec invalid format : " << e.DumpToString());
		return E_SIG_INVALID_FORMAT;
	} catch (const XmlSec::Exception::InvalidSig &e) {
		LogError("XmlSec invalid signature : " << e.DumpToString());
		return E_SIG_INVALID_SIG;
	} catch (const XmlSec::Exception::OutOfMemory &e) {
		LogError("XmlSec out of memory : " << e.DumpToString());
		return E_SIG_OUT_OF_MEM;
	} catch (const XmlSec::Exception::Base &e) {
		LogError("XmlSec unknown exception : " << e.DumpToString());
		return E_SIG_INVALID_FORMAT;
	} catch (const Ocsp::Exception::OcspUnsupported &e) {
		LogInfo("Ocsp unsupported : " << e.DumpToString());
	} catch (const Ocsp::Exception::Base &e) {
		LogInfo("Ocsp check throw exeption : " << e.DumpToString());
	} catch (const std::exception &e) {
		LogError("std exception occured : " << e.what());
		return E_SIG_UNKNOWN;
	} catch (...) {
		LogError("Unknown exception in BaseValidator::check");
		return E_SIG_UNKNOWN;
	}

	return m_disregarded ? E_SIG_DISREGARDED : E_SIG_NONE;
}

VCerr BaseValidator::baseCheckList(bool checkOcsp, const UriList &uriList)
{
	try {
		// Make certificate chain, check certificate info
		VCerr result = preStep();

		if (result != E_SIG_NONE)
			return result;

		// XmlSec validate
		if (uriList.size() == 0)
			m_xmlSec.validateNoHash(m_context);
		else
			m_xmlSec.validatePartialHash(m_context, uriList);

		if (checkOcsp && Ocsp::check(m_data) == Ocsp::Result::REVOKED) {
			LogError("Certificate is Revoked by OCSP server.");
			return E_SIG_REVOKED;
		}

		LogDebug("Signature validation of check list done successfully ");
	} catch (const CertificateCollection::Exception::Base &e) {
		LogError("CertificateCollection exception : " << e.DumpToString());
		return E_SIG_INVALID_CHAIN;
	} catch (const XmlSec::Exception::InternalError &e) {
		LogError("XmlSec internal error : " << e.DumpToString());
		return E_SIG_INVALID_FORMAT;
	} catch (const XmlSec::Exception::InvalidFormat &e) {
		LogError("XmlSec invalid format : " << e.DumpToString());
		return E_SIG_INVALID_FORMAT;
	} catch (const XmlSec::Exception::InvalidSig &e) {
		LogError("XmlSec invalid signature : " << e.DumpToString());
		return E_SIG_INVALID_SIG;
	} catch (const XmlSec::Exception::OutOfMemory &e) {
		LogError("XmlSec out of memory : " << e.DumpToString());
		return E_SIG_OUT_OF_MEM;
	} catch (const XmlSec::Exception::Base &e) {
		LogError("XmlSec unknown exception : " << e.DumpToString());
		return E_SIG_INVALID_FORMAT;
	} catch (const Ocsp::Exception::OcspUnsupported &e) {
		LogInfo("Ocsp unsupported : " << e.DumpToString());
	} catch (const Ocsp::Exception::Base &e) {
		LogInfo("Ocsp check throw exeption : " << e.DumpToString());
	} catch (...) {
		LogError("Unknown exception in BaseValidator::checkList");
		return E_SIG_UNKNOWN;
	}

	return m_disregarded ? E_SIG_DISREGARDED : E_SIG_NONE;
}

VCerr BaseValidator::makeChainBySignature(bool completeWithSystemCert,
										  CertificateList &certList)
{
	VCerr result = makeDataBySignature(completeWithSystemCert);

	if (result != E_SIG_NONE)
		return result;

	certList = m_data.getCertList();
	return E_SIG_NONE;
}

std::string BaseValidator::errorToString(VCerr code)
{
	switch (code) {
	case E_SIG_NONE:
		return "Success.";

	case E_SIG_INVALID_FORMAT:
		return "Invalid format of signature file.";

	case E_SIG_INVALID_CERT:
		return "Invalid format of certificate in signature.";

	case E_SIG_INVALID_CHAIN:
		return "Invalid certificate chain with certificate in signature.";

	case E_SIG_INVALID_SIG:
		return "Invalid signature. Signed with wrong key, changed signature file or changed package file.";

	case E_SIG_INVALID_REF:
		return "Invalid file reference. An unsinged file was found.";

	case E_SIG_CERT_EXPIRED:
		return "Certificate in signature was expired.";

	case E_SIG_CERT_NOT_YET:
		return "Certificate in signature is not valid yet.";

	case E_SIG_DISREGARDED:
		return "Signature validation can be disregarded in some cases.";

	case E_SIG_REVOKED:
		return "One of certificate was revoked in certificate chain.";

	case E_SIG_PLUGIN:
		return "Failed to load plugin for additional validation check.";

	case E_SIG_OUT_OF_MEM:
		return "Out of memory.";

	case E_SIG_UNKNOWN:
		return "Unknown error.";

	default:
		return m_pluginHandler.errorToString(code);
	}
}

} // namespace ValidationCore
