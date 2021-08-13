/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        signature-validator-since-4.0.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Signature Validator example (Tizen_4.0 ~ )
 */
#include <string>
#include <iostream>

#include <vcore/SignatureValidator.h>

const std::string PKG_PATH(CERT_SVC_EXAMPLES "/resource/player");

int main()
{
	// Step 1. Validate signature files.
	ValidationCore::SignatureValidator validator(PKG_PATH);
	ValidationCore::SignatureDataMap sigDataMap;
	ValidationCore::VCerr result = validator.checkAll(true, // OCSP
													  true, // reverse reference check
													  sigDataMap);
	/*
		If you want to validate specific files, use checkListAll().

		ex) UriList uriList;
			uriList.emplace_back("author-siganture.xml");
			uriList.emplace_back("res/res.xml");
			uriList.emplace_back("shared/res/preference.png");
			uriList.emplace_back("tizen-manifest.xml");

			validator.checkListAll(true, // OCSP
								   uriList,
								   data);
	*/

	switch (result) {
	case ValidationCore::E_SIG_DISREGARDED:
		std::cout << "This signature is disregarded." << std::endl;
		break;

	case ValidationCore::E_SIG_NONE:
		std::cout << "Success to validate signature." << std::endl;
		break;

	default:
		std::cout << "Failed to validate signature : "
				  << validator.errorToString(result) << std::endl;
		return -1;
	}

	// Step 2. Get certificate list from signature.
	auto authorSigData = sigDataMap[ValidationCore::SignatureType::AUTHOR];
	for (const auto &certPtr : authorSigData.getCertList())
		std::cout << "Author certificate > "
				  << certPtr->getBase64() << std::endl;

	auto dist1SigData = sigDataMap[ValidationCore::SignatureType::DISTRIBUTOR1];
	for (const auto &certPtr : dist1SigData.getCertList())
		std::cout << "Distributor1 certificate > "
				  << certPtr->getBase64() << std::endl;

	auto dist2SigData = sigDataMap[ValidationCore::SignatureType::DISTRIBUTOR2];
	for (const auto &certPtr : dist2SigData.getCertList())
		std::cout << "Distributor2 certificate > "
				  << certPtr->getBase64() << std::endl;

	// Step 3. Check visibility about distributor1.
	switch (dist1SigData.getVisibilityLevel()) {
	case ValidationCore::CertStoreId::VIS_PUBLIC:
		std::cout << "This signature has public visibility." << std::endl;
		break;
	case ValidationCore::CertStoreId::VIS_PARTNER:
		std::cout << "This signature has partner visibility." << std::endl;
		break;
	case ValidationCore::CertStoreId::VIS_PLATFORM:
		std::cout << "This signature has platform visibility." << std::endl;
		break;
	default:
		std::cout << "Failed to get proper visibility." << std::endl;
		return -1;
	}

	return 0;
}
