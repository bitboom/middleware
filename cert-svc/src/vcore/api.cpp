/**
 * Copyright (c) 2016 - 2019 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        api.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @author      Jacek Migacz (j.migacz@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       This is part of C-api proposition for cert-svc.
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <algorithm>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "dpl/log/log.h"

#include "vcore/Base64.h"
#include "vcore/Certificate.h"
#include "vcore/CertificateCollection.h"
#include "vcore/pkcs12.h"
#include "vcore/Client.h"
#include "vcore/ValidatorFactories.h"

#include "cert-svc/cinstance.h"
#include "cert-svc/ccert.h"
#include "cert-svc/cpkcs12.h"
#include "cert-svc/cprimitives.h"

#define START_CERT      "-----BEGIN CERTIFICATE-----"
#define END_CERT        "-----END CERTIFICATE-----"
#define START_TRUSTED   "-----BEGIN TRUSTED CERTIFICATE-----"
#define END_TRUSTED     "-----END TRUSTED CERTIFICATE-----"

using namespace ValidationCore;

namespace {

struct CharDeleter {
	void operator()(char *str) const
	{
		free(str);
	}
};

struct CharArrDeleter {
	void operator()(char **arr) const
	{
		size_t i = 0;

		if (arr == NULL)
			return;

		while (arr[i])
			free(arr[i++]);

		free(arr);
	}
};

class CertSvcInstanceImpl {
public:
	CertSvcInstanceImpl()
		: m_certificateCounter(0)
		, m_idListCounter(0)
		, m_stringListCounter(0)
	{}

	~CertSvcInstanceImpl()
	{
		auto it = m_allocatedStringSet.begin();

		for (; it != m_allocatedStringSet.end(); ++it)
			delete[] *it;
	}

	inline void reset()
	{
		m_certificateCounter = 0;
		m_certificateMap.clear();
		m_idListCounter = 0;
		m_idListMap.clear();
		m_stringListCounter = 0;
		m_stringListMap.clear();
		auto it = m_allocatedStringSet.begin();

		for (; it != m_allocatedStringSet.end(); ++it)
			delete[] *it;

		m_allocatedStringSet.clear();
	}

	inline size_t addCert(const CertificatePtr &cert)
	{
		m_certificateMap[m_certificateCounter] = cert;
		return m_certificateCounter++;
	}

	inline void removeCert(const CertSvcCertificate &cert)
	{
		auto iter = m_certificateMap.find(cert.privateHandler);

		if (iter != m_certificateMap.end()) {
			m_certificateMap.erase(iter);
		}
	}

	inline int getCertFromList(
		const CertSvcCertificateList &handler,
		size_t position,
		CertSvcCertificate *certificate)
	{
		auto iter = m_idListMap.find(handler.privateHandler);

		if (iter == m_idListMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		if (position >= iter->second.size()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		certificate->privateInstance = handler.privateInstance;
		certificate->privateHandler = (iter->second)[position];
		return CERTSVC_SUCCESS;
	}

	inline int getCertListLen(const CertSvcCertificateList &handler, size_t *len)
	{
		auto iter = m_idListMap.find(handler.privateHandler);

		if (iter == m_idListMap.end() || !len) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		*len = (iter->second).size();
		return CERTSVC_SUCCESS;
	}

	inline void removeCertList(const CertSvcCertificateList &handler)
	{
		auto iter = m_idListMap.find(handler.privateHandler);

		if (iter != m_idListMap.end())
			m_idListMap.erase(iter);
	}

	inline void removeCertListAll(const CertSvcCertificateList &handler)
	{
		auto iter = m_idListMap.find(handler.privateHandler);

		if (iter == m_idListMap.end())
			return;

		for (size_t pos = 0; pos < iter->second.size(); ++pos) {
			auto iterCert = m_certificateMap.find((iter->second)[pos]);

			if (iterCert == m_certificateMap.end())
				return;

			m_certificateMap.erase(iterCert);
		}

		m_idListMap.erase(iter);
	}

	inline int isSignedBy(const CertSvcCertificate &child,
						  const CertSvcCertificate &parent,
						  int *status)
	{
		auto citer = m_certificateMap.find(child.privateHandler);

		if (citer == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		auto piter = m_certificateMap.find(parent.privateHandler);

		if (piter == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		if (citer->second->isSignedBy(piter->second)) {
			*status = CERTSVC_TRUE;
		} else {
			*status = CERTSVC_FALSE;
		}

		return CERTSVC_SUCCESS;
	}

	inline int getField(const CertSvcCertificate &cert,
						CertSvcCertificateField field,
						CertSvcString *buffer)
	{
		auto iter = m_certificateMap.find(cert.privateHandler);

		if (iter == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		auto certPtr = iter->second;
		std::string result;

		switch (field) {
		case CERTSVC_SUBJECT:
			result = certPtr->getOneLine();
			break;

		case CERTSVC_ISSUER:
			result = certPtr->getOneLine(Certificate::FIELD_ISSUER);
			break;

		case CERTSVC_SUBJECT_COMMON_NAME:
			result = certPtr->getCommonName();
			break;

		case CERTSVC_SUBJECT_COUNTRY_NAME:
			result = certPtr->getCountryName();
			break;

		case CERTSVC_SUBJECT_STATE_NAME:
			result = certPtr->getStateOrProvinceName();
			break;

		case CERTSVC_SUBJECT_LOCALITY_NAME:
			result = certPtr->getLocalityName();
			break;

		case CERTSVC_SUBJECT_ORGANIZATION_NAME:
			result = certPtr->getOrganizationName();
			break;

		case CERTSVC_SUBJECT_ORGANIZATION_UNIT_NAME:
			result = certPtr->getOrganizationalUnitName();
			break;

		case CERTSVC_SUBJECT_EMAIL_ADDRESS:
			result = certPtr->getEmailAddres();
			break;

		/*
		            case CERTSVC_SUBJECT_UID:
		                result = certPtr->getUID();
		                break;
		*/
		case CERTSVC_ISSUER_COMMON_NAME:
			result = certPtr->getCommonName(Certificate::FIELD_ISSUER);
			break;

		case CERTSVC_ISSUER_COUNTRY_NAME:
			result = certPtr->getCountryName(Certificate::FIELD_ISSUER);
			break;

		case CERTSVC_ISSUER_STATE_NAME:
			result = certPtr->getStateOrProvinceName(Certificate::FIELD_ISSUER);
			break;

		case CERTSVC_ISSUER_LOCALITY_NAME:
			result = certPtr->getLocalityName(Certificate::FIELD_ISSUER);
			break;

		case CERTSVC_ISSUER_ORGANIZATION_NAME:
			result = certPtr->getOrganizationName(Certificate::FIELD_ISSUER);
			break;

		case CERTSVC_ISSUER_ORGANIZATION_UNIT_NAME:
			result = certPtr->getOrganizationalUnitName(Certificate::FIELD_ISSUER);
			break;

		case CERTSVC_ISSUER_EMAIL_ADDRESS:
			result = certPtr->getEmailAddres(Certificate::FIELD_ISSUER);
			break;

		/*
		            case CERTSVC_ISSUER_UID:
		                result = certPtr->getUID(Certificate::FIELD_ISSUER);
		                break;
		*/
		case CERTSVC_VERSION: {
			std::stringstream stream;
			stream << (certPtr->getVersion() + 1);
			result = stream.str();
			break;
		}

		case CERTSVC_SERIAL_NUMBER:
			result = certPtr->getSerialNumberString();
			break;

		case CERTSVC_KEY_USAGE:
			result = certPtr->getKeyUsageString();
			break;

		case CERTSVC_KEY:
			result = certPtr->getPublicKeyString();
			break;

		case CERTSVC_KEY_ALGO:
			result = certPtr->getPublicKeyAlgoString();
			break;

		case CERTSVC_SIGNATURE_ALGORITHM:
			result = certPtr->getSignatureAlgorithmString();
			break;

		default:
			break;
		}

		if (result.empty()) {
			buffer->privateHandler = NULL;
			buffer->privateLength = 0;
			buffer->privateInstance = cert.privateInstance;
			return CERTSVC_SUCCESS;
		}

		char *cstring = new char[result.size() + 1];

		if (cstring == NULL) {
			buffer->privateHandler = NULL;
			buffer->privateLength = 0;
			buffer->privateInstance = cert.privateInstance;
			return CERTSVC_BAD_ALLOC;
		}

		strncpy(cstring, result.c_str(), result.size() + 1);
		buffer->privateHandler = cstring;
		buffer->privateLength = result.size();
		buffer->privateInstance = cert.privateInstance;
		m_allocatedStringSet.insert(cstring);
		return CERTSVC_SUCCESS;
	}

	inline int getNotAfter(const CertSvcCertificate &cert,
						   time_t *time)
	{
		auto iter = m_certificateMap.find(cert.privateHandler);

		if (iter == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		*time = iter->second->getNotAfter();
		return CERTSVC_SUCCESS;
	}

	inline int getNotBefore(const CertSvcCertificate &cert,
							time_t *time)
	{
		auto iter = m_certificateMap.find(cert.privateHandler);

		if (iter == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		*time = iter->second->getNotBefore();
		return CERTSVC_SUCCESS;
	}

	inline int isRootCA(const CertSvcCertificate &cert, int *status)
	{
		auto iter = m_certificateMap.find(cert.privateHandler);

		if (iter == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		if (iter->second->isRootCert()) {
			*status = CERTSVC_TRUE;
		} else {
			*status = CERTSVC_FALSE;
		}

		return CERTSVC_SUCCESS;
	}

	inline int getStringFromList(
		const CertSvcStringList &handler,
		size_t position,
		CertSvcString *buffer)
	{
		buffer->privateHandler = NULL;
		buffer->privateLength = 0;
		auto iter = m_stringListMap.find(handler.privateHandler);

		if (iter == m_stringListMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		if (position >= iter->second.size()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		const std::string &data = iter->second.at(position);
		size_t size = data.size();
		char *cstring = new char[size + 1];

		if (!cstring) {
			return CERTSVC_FAIL;
		}

		strncpy(cstring, data.c_str(), size + 1);
		buffer->privateHandler = cstring;
		buffer->privateLength = size;
		buffer->privateInstance = handler.privateInstance;
		m_allocatedStringSet.insert(cstring);
		return CERTSVC_SUCCESS;
	}

	inline int getStringListLen(
		const CertSvcStringList &handler,
		size_t *size)
	{
		auto iter = m_stringListMap.find(handler.privateHandler);

		if (iter == m_stringListMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		*size = iter->second.size();
		return CERTSVC_SUCCESS;
	}

	inline void removeStringList(const CertSvcStringList &handler)
	{
		m_stringListMap.erase(m_stringListMap.find(handler.privateHandler));
	}

	inline void removeString(const CertSvcString &handler)
	{
		auto iter = m_allocatedStringSet.find(handler.privateHandler);

		if (iter != m_allocatedStringSet.end()) {
			delete[] *iter;
			m_allocatedStringSet.erase(iter);
		}
	}

	inline int sortCollection(CertSvcCertificate *certificate_array, size_t size)
	{
		if (size < 2) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		for (size_t i = 1; i < size; ++i) {
			if (certificate_array[i - 1].privateInstance.privatePtr
					!= certificate_array[i].privateInstance.privatePtr) {
				return CERTSVC_WRONG_ARGUMENT;
			}
		}

		CertificateList certList;
		std::map<Certificate *, size_t> translator;

		for (size_t i = 0; i < size; ++i) {
			size_t pos = certificate_array[i].privateHandler;
			auto cert = m_certificateMap.find(pos);

			if (cert == m_certificateMap.end()) {
				return CERTSVC_WRONG_ARGUMENT;
			}

			translator[cert->second.get()] = pos;
			certList.push_back(cert->second);
		}

		CertificateCollection collection;
		collection.load(certList);

		if (!collection.sort()) {
			return CERTSVC_FAIL;
		}

		auto chain = collection.getChain();
		size_t i = 0;

		for (const auto &cert : collection.getChain())
			certificate_array[i++].privateHandler = translator[cert.get()];

		return CERTSVC_SUCCESS;
	}

	inline int getX509Copy(const CertSvcCertificate &certificate, X509 **cert)
	{
		auto it = m_certificateMap.find(certificate.privateHandler);

		if (it == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		*cert = X509_dup(it->second->getX509());
		return CERTSVC_SUCCESS;
	}

	inline int getPubkeyDER(const CertSvcCertificate &certificate,
							unsigned char **pubkey,
							size_t *len)
	{
		auto it = m_certificateMap.find(certificate.privateHandler);

		if (it == m_certificateMap.end() || pubkey == NULL || len == NULL)
			return CERTSVC_WRONG_ARGUMENT;

		it->second->getPublicKeyDER(pubkey, len);
		return CERTSVC_SUCCESS;
	}

	inline int saveToFile(const CertSvcCertificate &certificate,
						  const char *location)
	{
		auto it = m_certificateMap.find(certificate.privateHandler);

		if (it == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		FILE *out = fopen(location, "w");

		if (out == NULL) {
			return CERTSVC_FAIL;
		}

		if (0 == i2d_X509_fp(out, it->second->getX509())) {
			fclose(out);
			return CERTSVC_FAIL;
		}

		fclose(out);
		return CERTSVC_SUCCESS;
	}

	inline int verify(
		CertSvcCertificate certificate,
		CertSvcString &message,
		CertSvcString &signature,
		const char *algorithm,
		int *status)
	{
		int result = CERTSVC_FAIL;

		if (!status) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		auto it = m_certificateMap.find(certificate.privateHandler);

		if (it == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		OpenSSL_add_all_digests();
		int temp;
		EVP_MD_CTX *mdctx = NULL;
		const EVP_MD *md = NULL;
		X509 *cert = it->second->getX509();
		EVP_PKEY *pkey = NULL;

		if (cert == NULL) {
			goto err;
		}

		pkey = X509_get_pubkey(cert);

		if (pkey == NULL) {
			goto err;
		}

		if (algorithm == NULL) {
			md = EVP_get_digestbynid(X509_get_signature_nid(cert));
		} else {
			md = EVP_get_digestbyname(algorithm);
		}

		if (md == NULL) {
			result = CERTSVC_INVALID_ALGORITHM;
			goto err;
		}

		mdctx = EVP_MD_CTX_create();

		if (mdctx == NULL) {
			goto err;
		}

		if (EVP_VerifyInit_ex(mdctx, md, NULL) != 1) {
			goto err;
		}

		if (EVP_VerifyUpdate(mdctx, message.privateHandler, message.privateLength) != 1) {
			goto err;
		}

		temp = EVP_VerifyFinal(mdctx,
							   reinterpret_cast<unsigned char *>(signature.privateHandler),
							   signature.privateLength,
							   pkey);

		if (temp == 0) {
			*status = CERTSVC_INVALID_SIGNATURE;
			result = CERTSVC_SUCCESS;
		} else if (temp == 1) {
			*status = CERTSVC_SUCCESS;
			result = CERTSVC_SUCCESS;
		}

err:

		if (mdctx != NULL)
			EVP_MD_CTX_destroy(mdctx);

		if (pkey != NULL)
			EVP_PKEY_free(pkey);

		return result;
	}

	inline int base64Encode(
		const CertSvcString &message,
		CertSvcString *base64)
	{
		if (!base64) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		std::string info(message.privateHandler, message.privateLength);
		Base64Encoder base;
		base.reset();
		base.append(info);
		base.finalize();
		info = base.get();
		char *ptr = new char[info.size() + 1];

		if (ptr == NULL) {
			return CERTSVC_BAD_ALLOC;
		}

		memcpy(ptr, info.c_str(), info.size() + 1);
		m_allocatedStringSet.insert(ptr);
		base64->privateHandler = ptr;
		base64->privateLength = info.size();
		base64->privateInstance = message.privateInstance;
		return CERTSVC_SUCCESS;
	}

	int base64Decode(
		const CertSvcString &base64,
		CertSvcString *message)
	{
		if (!message) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		std::string info(base64.privateHandler, base64.privateLength);
		Base64Decoder base;
		base.reset();
		base.append(info);

		if (!base.finalize()) {
			return CERTSVC_FAIL;
		}

		info = base.get();
		char *ptr = new char[info.size() + 1];

		if (ptr == NULL) {
			return CERTSVC_BAD_ALLOC;
		}

		memcpy(ptr, info.c_str(), info.size() + 1);
		m_allocatedStringSet.insert(ptr);
		message->privateHandler = ptr;
		message->privateLength = info.size();
		message->privateInstance = base64.privateInstance;
		return CERTSVC_SUCCESS;
	}

	inline int stringNew(
		CertSvcInstance &instance,
		const char *str,
		size_t size,
		CertSvcString *output)
	{
		if (!output)
			return CERTSVC_WRONG_ARGUMENT;

		/* return struct for empty string */
		if (size == 0 || str == NULL) {
			output->privateHandler = NULL;
			output->privateLength = 0;
			output->privateInstance = instance;
			return CERTSVC_SUCCESS;
		}

		if (strlen(str) < size)
			return CERTSVC_WRONG_ARGUMENT;

		char *ptr = new(std::nothrow) char[size + 1];

		if (ptr == NULL)
			return CERTSVC_BAD_ALLOC;

		memcpy(ptr, str, size);
		ptr[size] = '\0';
		output->privateHandler = ptr;
		output->privateLength = size;
		output->privateInstance = instance;
		m_allocatedStringSet.insert(ptr);
		return CERTSVC_SUCCESS;
	}

	inline int certificateVerify(
		CertSvcCertificate certificate,
		const CertSvcCertificate *trusted,
		size_t trustedSize,
		const CertSvcCertificate *untrusted,
		size_t untrustedSize,
		int checkCaFlag,
		int *status)
	{
		if (!trusted || !status) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		auto iter = m_certificateMap.find(certificate.privateHandler);

		if (iter == m_certificateMap.end()) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		X509 *cert = iter->second->getX509();
		X509_STORE *store = X509_STORE_new();
		STACK_OF(X509) *ustore = sk_X509_new_null();

		for (size_t i = 0; i < trustedSize; ++i) {
			auto iter = m_certificateMap.find(trusted[i].privateHandler);

			if (iter == m_certificateMap.end()) {
				X509_STORE_free(store);
				sk_X509_free(ustore);
				return CERTSVC_WRONG_ARGUMENT;
			}

			X509_STORE_add_cert(store, iter->second->getX509());
		}

		for (size_t i = 0; i < untrustedSize; ++i) {
			auto iter = m_certificateMap.find(untrusted[i].privateHandler);

			if (iter == m_certificateMap.end()) {
				X509_STORE_free(store);
				sk_X509_free(ustore);
				return CERTSVC_WRONG_ARGUMENT;
			}

			if (sk_X509_push(ustore, iter->second->getX509()) == 0)
				break;
		}

		X509_STORE_CTX *context;
		context = X509_STORE_CTX_new();
		if(!context) {
			X509_STORE_free(store);
			sk_X509_free(ustore);
			return CERTSVC_FAIL;
		}
		if(!X509_STORE_CTX_init(context, store, cert, ustore)) {
			X509_STORE_free(store);
			sk_X509_free(ustore);
			X509_STORE_CTX_free(context);
			return CERTSVC_FAIL;
		}
		int result = X509_verify_cert(context);

		if (result == 1 && checkCaFlag) { // check strictly
			STACK_OF(X509) *resultChain = X509_STORE_CTX_get1_chain(context);

			// the last one is not a CA.
			while (sk_X509_num(resultChain) > 1) {
				X509 *tmpCert = sk_X509_pop(resultChain);
				int caFlagValidity = X509_check_ca(tmpCert);
				X509_free(tmpCert);

				if (caFlagValidity != 1) {
					result = 0;
					break;
				}
			}
			sk_X509_pop_free(resultChain, X509_free);
		}

		X509_STORE_CTX_cleanup(context);
		X509_STORE_free(store);
		sk_X509_free(ustore);
		X509_STORE_CTX_free(context);

		if (result == 1) {
			*status = CERTSVC_SUCCESS;
		} else {
			*status = CERTSVC_FAIL;
		}

		return CERTSVC_SUCCESS;
	}

	int getVisibility(CertSvcCertificate certificate, CertSvcVisibility *visibility)
	{
		if (visibility == NULL)
			return CERTSVC_WRONG_ARGUMENT;

		auto iter = m_certificateMap.find(certificate.privateHandler);
		if (iter == m_certificateMap.end()) {
			LogError("Failed to find certificate.");
			return CERTSVC_FAIL;
		}

		auto certPtr = iter->second;
		auto storeIdSet = createCertificateIdentifier().find(certPtr);
		if (storeIdSet.contains(CERTSVC_VISIBILITY_PUBLIC))
			*visibility = CERTSVC_VISIBILITY_PUBLIC;
		else if (storeIdSet.contains(CERTSVC_VISIBILITY_PLATFORM))
			*visibility = CERTSVC_VISIBILITY_PLATFORM;
		else if (storeIdSet.contains(CERTSVC_VISIBILITY_PARTNER))
			*visibility = CERTSVC_VISIBILITY_PARTNER;
		else if (storeIdSet.contains(CERTSVC_VISIBILITY_DEVELOPER))
			*visibility = CERTSVC_VISIBILITY_DEVELOPER;
		else
			return CERTSVC_FAIL;

		LogInfo("Certificate's finger print : " <<
				Certificate::FingerprintToColonHex(certPtr->getFingerprint(
												   Certificate::FINGERPRINT_SHA1)) <<
				", visibility : " << *visibility);

		return CERTSVC_SUCCESS;
	}

	inline int pkcsNameIsUniqueInStore(
		CertStoreType storeType,
		CertSvcString pfxIdString,
		int *is_unique)
	{
		return vcore_client_check_alias_exist_in_store(storeType, pfxIdString.privateHandler, is_unique);
	}

	inline int getCertDetailFromStore(CertStoreType storeType,
									  CertSvcString gname,
									  char **certBuffer,
									  size_t *certSize)
	{
		return vcore_client_get_certificate_from_store(storeType, gname.privateHandler, certBuffer,
				certSize, PEM_CRT);
	}

	inline int pkcsDeleteCertFromStore(
		CertStoreType storeType,
		CertSvcString gname
	)
	{
		return vcore_client_delete_certificate_from_store(storeType, gname.privateHandler);
	}

	inline int pkcsHasPassword(
		CertSvcString filepath,
		int *has_password)
	{
		return pkcs12_has_password(filepath.privateHandler, has_password);
	}

	inline int pkcsImportToStore(
		CertStoreType storeType,
		CertSvcString path,
		CertSvcString pass,
		CertSvcString pfxIdString,
		CertSvcStoreCertList **certList,
		size_t *length)
	{
		return pkcs12_import_from_file_to_store(storeType, path.privateHandler, pass.privateHandler,
												pfxIdString.privateHandler, certList, length);
	}

	inline int pkcsGetAliasNameForCertInStore(CertStoreType storeType,
			CertSvcString gname,
			char **alias)
	{
		return vcore_client_get_certificate_alias_from_store(storeType, gname.privateHandler, alias);
	}

	inline int pkcsSetCertStatusToStore(CertStoreType storeType,
										int is_root_app,
										CertSvcString gname,
										CertStatus status)
	{
		return vcore_client_set_certificate_status_to_store(storeType, is_root_app, gname.privateHandler,
				status);
	}

	inline int pkcsGetCertStatusFromStore(
		CertStoreType storeType,
		CertSvcString gname,
		CertStatus *status)
	{
		return vcore_client_get_certificate_status_from_store(storeType, gname.privateHandler, status);
	}

	inline int getCertFromStore(CertSvcInstance instance,
								CertStoreType storeType,
								const char *gname,
								CertSvcCertificate *certificate)
	{
		return certsvc_get_certificate(instance, storeType, gname, certificate);
	}

	inline int freePkcsIdListFromStore(CertSvcStoreCertList **certList)
	{
		CertSvcStoreCertList *current;
		CertSvcStoreCertList *next;

		for (current = *certList; current != NULL; current = next) {
			next = current->next;
			free(current->title);
			free(current->gname);
			free(current);
		}

		*certList = NULL;
		return CERTSVC_SUCCESS;
	}

	inline int getPkcsIdListFromStore(
		CertStoreType storeType,
		int is_root_app,
		CertSvcStoreCertList **certList,
		size_t *length)
	{
		return vcore_client_get_certificate_list_from_store(storeType, is_root_app, certList, length);
	}

	inline int getPkcsIdEndUserListFromStore(
		CertStoreType storeType,
		CertSvcStoreCertList **certList,
		size_t *length)
	{
		return vcore_client_get_end_user_certificate_list_from_store(storeType, certList, length);
	}

	inline int getPkcsIdRootListFromStore(
		CertStoreType storeType,
		CertSvcStoreCertList **certList,
		size_t *length)
	{
		return vcore_client_get_root_certificate_list_from_store(storeType, certList, length);
	}

	inline int getPkcsPrivateKeyFromStore(
		CertStoreType storeType,
		CertSvcString gname,
		char **certBuffer,
		size_t *certSize)
	{
		return vcore_client_get_certificate_from_store(storeType, gname.privateHandler, certBuffer,
				certSize, (CertType)P12_PKEY);
	}

	inline int getPkcsCertificateListFromStore(
		CertSvcInstance &instance,
		CertStoreType storeType,
		CertSvcString &pfxIdString,
		CertSvcCertificateList *handler)
	{
		char **certs = NULL;
		size_t ncerts = 0;
		int result = vcore_client_load_certificates_from_store(storeType, pfxIdString.privateHandler,
					 &certs, &ncerts);

		if (result != CERTSVC_SUCCESS) {
			LogError("Unable to load certificates from store.");
			return result;
		}

		std::shared_ptr<char *> certsPtr(certs, CharArrDeleter());
		std::vector<CertificatePtr> certPtrVector;

		for (size_t i = 0; i < ncerts; i++) {
			CertSvcString Alias;
			Alias.privateHandler = certs[i];
			Alias.privateLength = strlen(certs[i]);
			char *certBuffer = NULL;
			size_t certLength = 0;
			result = certsvc_pkcs12_get_certificate_info_from_store(instance, storeType, Alias, &certBuffer,
					 &certLength);

			if (result != CERTSVC_SUCCESS || !certBuffer) {
				LogError("Failed to get certificate buffer.");
				return CERTSVC_FAIL;
			}

			std::shared_ptr<char> certBufferPtr(certBuffer, CharDeleter());
			const char *header = strstr(certBuffer, START_CERT);
			const char *headEnd = START_CERT;

			if (!header) {
				header = strstr(certBuffer, START_TRUSTED);
				headEnd = START_TRUSTED;

				if (!header) {
					LogError("Invalid format of certificate. alias : " << certs[i]);
					return CERTSVC_FAIL;
				}
			}

			const char *trailer = strstr(header, END_CERT);
			const char *tailEnd = END_CERT;

			if (!trailer) {
				trailer = strstr(header, END_TRUSTED);
				tailEnd = END_TRUSTED;

				if (!trailer) {
					LogError("Invalid format of certificate. alias : " << certs[i]);
					return CERTSVC_FAIL;
				}
			}

			if ((strcmp(headEnd, START_CERT) == 0 && strcmp(tailEnd, END_TRUSTED) == 0)
					|| (strcmp(headEnd, START_TRUSTED) == 0 && strcmp(tailEnd, END_CERT) == 0)) {
				LogError("Invalid format of certificate. alias : " << certs[i]);
				return CERTSVC_FAIL;
			}

			header += strlen(headEnd); /* cut headEnd */
			std::string certStr(header, strlen(header) - strlen(trailer));
			certPtrVector.push_back(CertificatePtr(new Certificate(certStr, Certificate::FORM_BASE64)));
		}

		std::vector<size_t> listId;

		for (const auto &cert : certPtrVector)
			listId.push_back(addCert(cert));

		size_t position = m_idListCounter++;
		m_idListMap[position] = listId;
		handler->privateInstance = instance;
		handler->privateHandler = position;
		return result;
	}

	inline bool checkValidStoreType(CertStoreType storeType)
	{
		if (storeType >= VPN_STORE && storeType <= ALL_STORE)
			return true;
		else
			return false;
	}

private:
	size_t m_certificateCounter;
	std::map<size_t, CertificatePtr> m_certificateMap;

	size_t m_idListCounter;
	std::map<size_t, std::vector<size_t>> m_idListMap;

	size_t m_stringListCounter;
	std::map<size_t, std::vector<std::string>> m_stringListMap;

	std::set<char *> m_allocatedStringSet;
};

inline CertSvcInstanceImpl *impl(CertSvcInstance instance)
{
	return static_cast<CertSvcInstanceImpl *>(instance.privatePtr);
}

} // namespace anonymous

int certsvc_instance_new(CertSvcInstance *instance)
{
	static int init = 1;

	if (init) {
		OpenSSL_add_ssl_algorithms();
		OpenSSL_add_all_digests();
		init = 0;
	}

	try {
		instance->privatePtr =
			reinterpret_cast<void *>(new CertSvcInstanceImpl);

		if (instance->privatePtr)
			return CERTSVC_SUCCESS;
	} catch (std::bad_alloc &) {
		return CERTSVC_BAD_ALLOC;
	} catch (...) {}

	return CERTSVC_FAIL;
}

void certsvc_instance_reset(CertSvcInstance instance)
{
	impl(instance)->reset();
}

void certsvc_instance_free(CertSvcInstance instance)
{
	delete impl(instance);
}

int certsvc_certificate_new_from_file(
	CertSvcInstance instance,
	const char *location,
	CertSvcCertificate *certificate)
{
	try {
		CertificatePtr cert = Certificate::createFromFile(location);
		certificate->privateInstance = instance;
		certificate->privateHandler = impl(instance)->addCert(cert);
		return CERTSVC_SUCCESS;
		// TODO support for std exceptions
	} catch (std::bad_alloc &) {
		return CERTSVC_BAD_ALLOC;
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_certificate_new_from_memory(
	CertSvcInstance instance,
	const unsigned char *memory,
	size_t len,
	CertSvcCertificateForm form,
	CertSvcCertificate *certificate)
{
	try {
		Certificate::FormType formType;
		std::string binary((char *)memory, len);

		if (CERTSVC_FORM_DER == form) {
			formType = Certificate::FORM_DER;
		} else {
			formType = Certificate::FORM_BASE64;
		}

		CertificatePtr cert(new Certificate(binary, formType));
		certificate->privateInstance = instance;
		certificate->privateHandler = impl(instance)->addCert(cert);
		return CERTSVC_SUCCESS;
	} catch (std::bad_alloc &) {
		return CERTSVC_BAD_ALLOC;
	} catch (...) {}

	return CERTSVC_FAIL;
}

void certsvc_certificate_free(CertSvcCertificate certificate)
{
	if (certificate.privateHandler != 0)
		impl(certificate.privateInstance)->removeCert(certificate);
}

int certsvc_certificate_save_file(
	CertSvcCertificate certificate,
	const char *location)
{
	return impl(certificate.privateInstance)->saveToFile(certificate, location);
}

int certsvc_certificate_list_get_one(
	CertSvcCertificateList handler,
	size_t position,
	CertSvcCertificate *certificate)
{
	return impl(handler.privateInstance)->
		   getCertFromList(handler, position, certificate);
}

int certsvc_certificate_list_get_length(
	CertSvcCertificateList handler,
	size_t *size)
{
	return impl(handler.privateInstance)->getCertListLen(handler, size);
}

void certsvc_certificate_list_free(CertSvcCertificateList handler)
{
	impl(handler.privateInstance)->removeCertList(handler);
}

void certsvc_certificate_list_all_free(CertSvcCertificateList handler)
{
	impl(handler.privateInstance)->removeCertListAll(handler);
}

int certsvc_certificate_is_signed_by(
	CertSvcCertificate child,
	CertSvcCertificate parent,
	int *status)
{
	if (child.privateInstance.privatePtr == parent.privateInstance.privatePtr) {
		return impl(child.privateInstance)->isSignedBy(child, parent, status);
	}

	return CERTSVC_WRONG_ARGUMENT;
}

int certsvc_certificate_get_string_field(
	CertSvcCertificate certificate,
	CertSvcCertificateField field,
	CertSvcString *buffer)
{
	try {
		return impl(certificate.privateInstance)->getField(certificate, field, buffer);
	} catch (std::bad_alloc &) {
		return CERTSVC_BAD_ALLOC;
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_certificate_get_not_after(
	CertSvcCertificate certificate,
	time_t *result)
{
	try {
		return impl(certificate.privateInstance)->getNotAfter(certificate, result);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_certificate_get_not_before(
	CertSvcCertificate certificate,
	time_t *result)
{
	try {
		return impl(certificate.privateInstance)->getNotBefore(certificate, result);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_certificate_is_root_ca(CertSvcCertificate certificate, int *status)
{
	return impl(certificate.privateInstance)->isRootCA(certificate, status);
}

int certsvc_string_list_get_one(
	CertSvcStringList handler,
	size_t position,
	CertSvcString *buffer)
{
	try {
		return impl(handler.privateInstance)->getStringFromList(handler, position, buffer);
	} catch (std::bad_alloc &) {
		return CERTSVC_BAD_ALLOC;
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_string_list_get_length(
	CertSvcStringList handler,
	size_t *size)
{
	return impl(handler.privateInstance)->getStringListLen(handler, size);
}

void certsvc_string_list_free(CertSvcStringList handler)
{
	if (handler.privateHandler != 0) {
		impl(handler.privateInstance)->removeStringList(handler);
		handler.privateHandler = 0;
	}
}

void certsvc_string_free(CertSvcString string)
{
	if (string.privateHandler)
		impl(string.privateInstance)->removeString(string);
}

void certsvc_string_to_cstring(
	CertSvcString string,
	const char **buffer,
	size_t *len)
{
	if (buffer) {
		*buffer = string.privateHandler;
	}

	if (len) {
		*len = string.privateLength;
	}
}

int certsvc_certificate_chain_sort(
	CertSvcCertificate *certificate_array,
	size_t size)
{
	try {
		if (!certificate_array) {
			return CERTSVC_WRONG_ARGUMENT;
		}

		return impl(certificate_array[0].privateInstance)->
			   sortCollection(certificate_array, size);
	} catch (std::bad_alloc &) {
		return CERTSVC_BAD_ALLOC;
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_certificate_dup_x509(CertSvcCertificate certificate, X509 **cert)
{
	try {
		return impl(certificate.privateInstance)->getX509Copy(certificate, cert);
	} catch (...) {}

	return CERTSVC_FAIL;
}

void certsvc_certificate_free_x509(X509 *x509)
{
	if (x509)
		X509_free(x509);
}

int certsvc_certificate_dup_pubkey_der(
	CertSvcCertificate certificate,
	unsigned char **pubkey,
	size_t *len)
{
	try {
		return impl(certificate.privateInstance)->getPubkeyDER(certificate, pubkey, len);
	} catch (...) {}

	return CERTSVC_FAIL;
}

void certsvc_pkcs12_free_evp_pkey(EVP_PKEY *pkey)
{
	EVP_PKEY_free(pkey);
}

int certsvc_message_verify(
	CertSvcCertificate certificate,
	CertSvcString message,
	CertSvcString signature,
	const char *algorithm,
	int *status)
{
	try {
		return impl(certificate.privateInstance)->verify(
				   certificate,
				   message,
				   signature,
				   algorithm,
				   status);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_base64_encode(CertSvcString message, CertSvcString *base64)
{
	try {
		return impl(message.privateInstance)->base64Encode(message, base64);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_base64_decode(CertSvcString base64, CertSvcString *message)
{
	try {
		return impl(base64.privateInstance)->base64Decode(base64, message);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_string_new(
	CertSvcInstance instance,
	const char *url,
	size_t size,
	CertSvcString *output)
{
	try {
		return impl(instance)->stringNew(instance, url, size, output);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_string_not_managed(
	CertSvcInstance instance,
	const char *url,
	size_t size,
	CertSvcString *output)
{
	if (!output) {
		return CERTSVC_WRONG_ARGUMENT;
	}

	output->privateHandler = const_cast<char *>(url);
	output->privateLength = size;
	output->privateInstance = instance;
	return CERTSVC_SUCCESS;
}

int certsvc_certificate_verify(
	CertSvcCertificate certificate,
	const CertSvcCertificate *trusted,
	size_t trustedSize,
	const CertSvcCertificate *untrusted,
	size_t untrustedSize,
	int *status)
{
	try {
		return impl(certificate.privateInstance)->certificateVerify(
				   certificate,
				   trusted,
				   trustedSize,
				   untrusted,
				   untrustedSize,
				   0,
				   status);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_certificate_verify_with_caflag(
	CertSvcCertificate certificate,
	const CertSvcCertificate *trusted,
	size_t trustedSize,
	const CertSvcCertificate *untrusted,
	size_t untrustedSize,
	int *status)
{
	try {
		return impl(certificate.privateInstance)->certificateVerify(
				   certificate,
				   trusted,
				   trustedSize,
				   untrusted,
				   untrustedSize,
				   1,
				   status);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_certificate_get_visibility(CertSvcCertificate certificate,
									   CertSvcVisibility *visibility)
{
	try {

		return impl(certificate.privateInstance)->getVisibility(certificate, visibility);

	} catch (...) {
		LogError("exception occur");
	}

	return CERTSVC_FAIL;
}

int certsvc_get_certificate(CertSvcInstance instance,
							CertStoreType storeType,
							const char *gname,
							CertSvcCertificate *certificate)
{
	try {
		size_t len = 0;
		char *certbuf = nullptr;
		int result = vcore_client_get_certificate_from_store(storeType, gname, &certbuf,
															 &len, PEM_CRT);

		if (result != CERTSVC_SUCCESS) {
			LogError("Failed to get certificate buffer from store.");
			return result;
		}
		std::unique_ptr<char, void(*)(void *)> certbufptr(certbuf, free);

		LogInfo("certbuf: " << certbuf);

		std::unique_ptr<BIO, int(*)(BIO *)> bioptr(BIO_new(BIO_s_mem()), BIO_free);
		if (bioptr == nullptr) {
			LogError("Failed to allocate memory.");
			return CERTSVC_BAD_ALLOC;
		}

		len = BIO_write(bioptr.get(), (const void *)certbuf, (int)len);

		if ((int)len < 1) {
			LogError("Failed to load cert into bio.");
			return CERTSVC_BAD_ALLOC;
		}

		std::unique_ptr<X509, void(*)(X509 *)> x509ptr(
				PEM_read_bio_X509_AUX(bioptr.get(), nullptr, nullptr, nullptr), X509_free);

		CertificatePtr cert;

		if (x509ptr != nullptr) {
			LogInfo("PEM_read_bio_X509_AUX returned x509 struct!");
			try {
				cert.reset(new Certificate(x509ptr.get()));
				LogInfo("Parse cert success with PEM(AUX) form!");
			} catch (...) {}
		}

		if (cert == nullptr) {
			x509ptr.reset(PEM_read_bio_X509(bioptr.get(), nullptr, nullptr, nullptr));
			if (x509ptr != nullptr) {
				LogInfo("PEM_read_bio_X509 returned x509 struct!");
				try {
					cert.reset(new Certificate(x509ptr.get()));
					LogInfo("Parse cert success with PEM form!");
				} catch (...) {}
			}
		}

		if (cert == nullptr) {
			try {
				cert.reset(new Certificate(certbuf, Certificate::FormType::FORM_BASE64));
				LogInfo("Parse cert success with Base64 form!");
			} catch (...) {}
		}

		if (cert == nullptr) {
			try {
				cert.reset(new Certificate(certbuf, Certificate::FormType::FORM_DER));
				LogInfo("Parse cert success with DER form!");
			} catch (...) {}
		}

		if (cert == nullptr) {
			LogError("Failed to parse cert on all of PEM/DER/Base64 form!");
			return CERTSVC_INVALID_CERTIFICATE;
		}

		certificate->privateInstance = instance;
		certificate->privateHandler = impl(instance)->addCert(cert);

		return CERTSVC_SUCCESS;
	} catch (std::bad_alloc &) {
		return CERTSVC_BAD_ALLOC;
	} catch (...) {
		return CERTSVC_FAIL;
	}
}

int certsvc_pkcs12_check_alias_exists_in_store(CertSvcInstance instance,
		CertStoreType storeType,
		CertSvcString pfxIdString,
		int *is_unique)
{
	if (pfxIdString.privateHandler == NULL || pfxIdString.privateLength <= 0) {
		LogError("Invalid input parameter.");
		return CERTSVC_WRONG_ARGUMENT;
	}

	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->pkcsNameIsUniqueInStore(storeType, pfxIdString, is_unique);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_free_certificate_list_loaded_from_store(CertSvcInstance instance,
		CertSvcStoreCertList **certList)
{
	if (certList == NULL || *certList == NULL) {
		LogError("Invalid input parameter.");
		return CERTSVC_WRONG_ARGUMENT;
	}

	try {
		return impl(instance)->freePkcsIdListFromStore(certList);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_get_certificate_list_from_store(CertSvcInstance instance,
		CertStoreType storeType,
		int is_root_app,
		CertSvcStoreCertList **certList,
		size_t *length)
{
	if (certList == NULL || *certList != NULL) {
		LogError("Invalid input parameter.");
		return CERTSVC_WRONG_ARGUMENT;
	}

	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->getPkcsIdListFromStore(storeType, is_root_app, certList, length);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_get_end_user_certificate_list_from_store(CertSvcInstance instance,
		CertStoreType storeType,
		CertSvcStoreCertList **certList,
		size_t *length)
{
	if (certList == NULL || *certList != NULL) {
		LogError("Invalid input parameter.");
		return CERTSVC_WRONG_ARGUMENT;
	}

	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->getPkcsIdEndUserListFromStore(storeType, certList, length);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_get_root_certificate_list_from_store(CertSvcInstance instance,
		CertStoreType storeType,
		CertSvcStoreCertList **certList,
		size_t *length)
{
	if (certList == NULL || *certList != NULL) {
		LogError("Invalid input parameter.");
		return CERTSVC_WRONG_ARGUMENT;
	}

	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->getPkcsIdRootListFromStore(storeType, certList, length);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_get_certificate_info_from_store(CertSvcInstance instance,
		CertStoreType storeType,
		CertSvcString gname,
		char **certBuffer,
		size_t *certSize)
{
	if (certBuffer == NULL || *certBuffer != NULL) {
		LogError("Invalid input parameter.");
		return CERTSVC_WRONG_ARGUMENT;
	}

	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->getCertDetailFromStore(storeType, gname, certBuffer, certSize);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_delete_certificate_from_store(CertSvcInstance instance,
		CertStoreType storeType,
		CertSvcString gname)
{
	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->pkcsDeleteCertFromStore(storeType, gname);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_import_from_file_to_store(CertSvcInstance instance,
		CertStoreType storeType,
		CertSvcString path,
		CertSvcString password,
		CertSvcString pfxIdString)
{
	try {
		if (path.privateHandler == NULL || !impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->pkcsImportToStore(storeType, path, password, pfxIdString,
												 NULL, NULL);
	} catch (...) {
		LogError("Exception occured from pkcsImportToStore");
		return CERTSVC_FAIL;
	}
}

int certsvc_pkcs12_import_from_file_to_store_ret_list(CertSvcInstance instance,
		CertStoreType storeType,
		CertSvcString path,
		CertSvcString password,
		CertSvcString pfxIdString,
		CertSvcStoreCertList **certList,
		size_t *length)
{
	try {
		if (certList == NULL || length == NULL) {
			LogError("Wrong argument.");
			return CERTSVC_WRONG_ARGUMENT;
		}

		if (path.privateHandler == NULL || !impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->pkcsImportToStore(storeType, path, password, pfxIdString,
												 certList, length);
	} catch (...) {
		LogError("Exception occured from pkcsImportToStore");
		return CERTSVC_FAIL;
	}
}

int certsvc_pkcs12_get_alias_name_for_certificate_in_store(CertSvcInstance instance,
		CertStoreType storeType,
		CertSvcString gname,
		char **alias)
{
	if (gname.privateHandler == NULL || gname.privateLength <= 0) {
		LogError("Invalid input parameter.");
		return CERTSVC_WRONG_ARGUMENT;
	}

	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->pkcsGetAliasNameForCertInStore(storeType, gname, alias);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_set_certificate_status_to_store(CertSvcInstance instance,
		CertStoreType storeType,
		int is_root_app,
		CertSvcString gname,
		CertStatus status)
{
	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->pkcsSetCertStatusToStore(storeType, is_root_app, gname, status);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_get_certificate_status_from_store(
	CertSvcInstance instance,
	CertStoreType storeType,
	CertSvcString gname,
	CertStatus *status)
{
	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->pkcsGetCertStatusFromStore(storeType, gname, status);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_get_certificate_from_store(CertSvcInstance instance,
		CertStoreType storeType,
		const char *gname,
		CertSvcCertificate *certificate)
{
	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->getCertFromStore(instance, storeType, gname, certificate);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_load_certificate_list_from_store(
	CertSvcInstance instance,
	CertStoreType storeType,
	CertSvcString pfxIdString,
	CertSvcCertificateList *certificateList)
{
	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->getPkcsCertificateListFromStore(instance, storeType, pfxIdString,
				certificateList);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_private_key_dup_from_store(
	CertSvcInstance instance,
	CertStoreType storeType,
	CertSvcString gname,
	char **certBuffer,
	size_t *certSize)
{
	try {
		if (!impl(instance)->checkValidStoreType(storeType)) {
			LogError("Invalid input parameter.");
			return CERTSVC_INVALID_STORE_TYPE;
		}

		return impl(instance)->getPkcsPrivateKeyFromStore(storeType, gname, certBuffer, certSize);
	} catch (...) {}

	return CERTSVC_FAIL;
}

int certsvc_pkcs12_dup_evp_pkey_from_store(
	CertSvcInstance instance,
	CertStoreType storeType,
	CertSvcString gname,
	EVP_PKEY **pkey)
{
	char *buffer = NULL;
	size_t size;
	int result = certsvc_pkcs12_private_key_dup_from_store(instance, storeType, gname, &buffer, &size);

	if (result != CERTSVC_SUCCESS) {
		LogError("Error in certsvc_pkcs12_private_key_dup");
		return result;
	}

	BIO *b = BIO_new(BIO_s_mem());

	if ((int)size != BIO_write(b, buffer, size)) {
		LogError("Error in BIO_write");
		BIO_free_all(b);
		certsvc_pkcs12_private_key_free(buffer);
		return CERTSVC_FAIL;
	}

	certsvc_pkcs12_private_key_free(buffer);
	*pkey = PEM_read_bio_PrivateKey(b, NULL, NULL, NULL);
	BIO_free_all(b);

	if (*pkey)
		return CERTSVC_SUCCESS;

	LogError("Result is null. Openssl REASON code is : " << ERR_GET_REASON(ERR_peek_last_error()));
	return CERTSVC_FAIL;
}

int certsvc_pkcs12_has_password(
	CertSvcInstance instance,
	CertSvcString filepath,
	int *has_password)
{
	try {
		return impl(instance)->pkcsHasPassword(filepath, has_password);
	} catch (...) {}

	return CERTSVC_FAIL;
}

void certsvc_pkcs12_private_key_free(char *buffer)
{
	free(buffer);
}

