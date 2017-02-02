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
 * @file        signature-validator-until-3.0.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Signature Validator example (~ Tizen_3.0)
 */
#include <string>
#include <iostream>

#include <vcore/SignatureValidator.h>

const std::string PKG_PATH(CERT_SVC_EXAMPLES "/resource/player");

int main()
{
	// Step 1. Find signature files in package.
	ValidationCore::SignatureFileInfoSet signatureSet;
	ValidationCore::SignatureFinder finder(PKG_PATH);
	if (ValidationCore::SignatureFinder::NO_ERROR != finder.find(signatureSet)) {
		std::cout << "Failed to find signature." << std::endl;
		return -1;
	}

	// Step 2. Validate signature files.
	for (const auto &signature : signatureSet) {
		ValidationCore::SignatureValidator validator(signature);
		ValidationCore::SignatureData data;
		ValidationCore::VCerr result = validator.check(PKG_PATH,
													   true, // OCSP
													   true, // reverse reference check
													   data);
		/*
			If you want to validate specific files, use checkList().

			ex) UriList uriList;
				uriList.emplace_back("author-siganture.xml");
				uriList.emplace_back("res/res.xml");
				uriList.emplace_back("shared/res/preference.png");
				uriList.emplace_back("tizen-manifest.xml");

				validator.checkList(true, // OCSP
									uriList,
									data);
		*/

		std::cout << "Start to validate : "
				  << data.getSignatureFileName() << std::endl;

		switch (result) {
		case ValidationCore::E_SIG_DISREGARDED:
			std::cout << "This signature is disregarded." << std::endl;
			break;

		case ValidationCore::E_SIG_NONE:
			std::cout << "Success to validate signature." << std::endl;
			// Step 3. Get certificate list from signature.
			for (const auto &certPtr : data.getCertList())
				std::cout << certPtr->getBase64() << std::endl;
			break;

		default:
			std::cout << "Failed to validate signature : "
					  << validator.errorToString(result) << std::endl;
			return -1;
		}

		// Step 4. Check visibility about distributor1.
		if (!data.isAuthorSignature() && data.getSignatureNumber() == 1) {
			switch (data.getVisibilityLevel()) {
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
		}
	}

	return 0;
}
