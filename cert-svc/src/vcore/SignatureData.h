/*
 * Copyright (c) 2016 - 2019 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        SignatureData.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       SignatureData is used to storage data parsed from digsig file.
 */
#ifndef _SIGNATUREDATA_H_
#define _SIGNATUREDATA_H_

#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <string>

#include <vcore/Certificate.h>
#include <vcore/CertStoreType.h>

namespace ValidationCore {

enum SignatureType {
	AUTHOR = -1,
	DISTRIBUTOR1 = 1,
	DISTRIBUTOR2 = 2
}; // Distributor type can be increase, do not use enum class.

class SignatureData;

typedef std::set<std::string> ReferenceSet;
typedef std::set<std::string> ProxySet;
typedef std::list<std::string> ObjectList;
typedef std::map<int, SignatureData> SignatureDataMap;

class SignatureData {
public:
	SignatureData();
	SignatureData(const std::string &fileName, int fileNumber);

	virtual ~SignatureData();

	typedef std::list<std::string> IMEIList;
	typedef std::list<std::string> MEIDList;

	void setReference(const ReferenceSet &referenceSet);
	void setSortedCertificateList(const CertificateList &list);
	void setStorageType(const CertStoreId::Set &storeIdSet);
	bool isCertListSorted() const;

	const ReferenceSet &getReferenceSet() const;
	CertificateList getCertList() const;
	ObjectList getObjectList() const;
	bool containObjectReference(const std::string &ref) const;
	bool isAuthorSignature() const;
	int getSignatureNumber() const;
	std::string getSignatureFileName() const;
	std::string getRoleURI() const;
	std::string getProfileURI() const;
	const CertStoreId::Set &getStorageType() const;
	CertStoreId::Type getVisibilityLevel() const;
	const IMEIList &getIMEIList() const;
	const MEIDList &getMEIDList() const;
	CertificatePtr getEndEntityCertificatePtr() const;
	CertificatePtr getRootCaCertificatePtr() const;
	void setExtSignatureParam(const std::string &key, const std::string value);
	bool getExtSignatureParam(const std::string &key, std::string &value);

	friend class SignatureReader;

private:
	ReferenceSet m_referenceSet;
	ProxySet m_proxySet;
	CertificateList m_certList;

	//TargetRestriction
	IMEIList m_imeiList;
	MEIDList m_meidList;

	/*
	 * This number is taken from distributor signature file name.
	 * Author signature do not contain any number on the file name.
	 * Author signature should have signature number equal to -1.
	 */
	int m_signatureNumber;
	std::string m_fileName;
	std::string m_roleURI;
	std::string m_profileURI;
	std::string m_identifier;
	ObjectList m_objectList;
	CertStoreId::Set m_storeIdSet;
	bool m_certificateSorted;

	//A map that can be used ie. by validator plugins to pass additional information to installer
	std::unordered_map<std::string, std::string> m_extSignatureParams;
};

} // ValidationCore

#endif
