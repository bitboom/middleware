/*
 *
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
#include <string>
#include <vector>
#include <iostream>

#include <dpl/test/test_runner.h>
#include <vcore/SignatureFinder.h>
#include <vcore/SignatureValidator.h>

#include "test-common.h"
#include "test-util.h"

using namespace ValidationCore;

namespace {

using RetPair = std::pair<VCerr, SignatureData>;

std::vector<RetPair> runCheck(const std::string &contentPath,
							  bool checkOcsp,
							  bool checkReferences)
{
	SignatureFileInfoSet signatureSet;
	SignatureFinder signatureFinder(contentPath);
	RUNNER_ASSERT_MSG(
		SignatureFinder::NO_ERROR == signatureFinder.find(signatureSet),
		"SignatureFinder failed");

	std::vector<RetPair> ret;
	for (auto &sig : signatureSet) {
		SignatureValidator validator(sig);
		SignatureData outData;
		ret.push_back(std::make_pair(validator.check(contentPath,
													 checkOcsp,
													 checkReferences,
													 outData),
									 outData));
	}
	return ret;
}

std::vector<RetPair> runCheckList(const std::string &contentPath,
								  bool checkOcsp,
								  const UriList &uriList)
{
	SignatureFileInfoSet signatureSet;
	SignatureFinder signatureFinder(contentPath);
	RUNNER_ASSERT_MSG(
		SignatureFinder::NO_ERROR == signatureFinder.find(signatureSet),
		"SignatureFinder failed");

	std::vector<RetPair> ret;
	for (auto &sig : signatureSet) {
		SignatureValidator validator(sig);
		SignatureData outData;
		ret.push_back(std::make_pair(validator.checkList(checkOcsp,
														 uriList,
														 outData),
									 outData));
	}
	return ret;
}

VCerr runCheckAll(const std::string &contentPath,
				  bool checkOcsp,
				  bool checkReferences)
{
	SignatureValidator validator(contentPath);
	SignatureDataMap sigDataMap;
	return validator.checkAll(checkOcsp, checkReferences, sigDataMap);
}

VCerr runCheckListAll(const std::string &contentPath,
					  bool checkOcsp,
					  const UriList &uriList)
{
	SignatureValidator validator(contentPath);
	SignatureDataMap sigDataMap;
	return validator.checkListAll(checkOcsp, uriList, sigDataMap);
}

} // anonymous namespace

RUNNER_TEST_GROUP_INIT(T0010_SIGNATURE_VALIDATOR)

RUNNER_TEST(T00101_finder)
{
	SignatureFileInfoSet signatureSet;
	SignatureFinder signatureFinder(TestData::widget_path);
	RUNNER_ASSERT_MSG(
		SignatureFinder::NO_ERROR == signatureFinder.find(signatureSet),
		"SignatureFinder failed");
	RUNNER_ASSERT_MSG(signatureSet.size() == 2, "Some signature has not been found");

	for (const auto &fileInfo : signatureSet)
		RUNNER_ASSERT_MSG(((fileInfo.getFileName().find("author-signature.xml") != std::string::npos &&
							fileInfo.getFileNumber() == -1) ||
						   (fileInfo.getFileName().find("signature1.xml") != std::string::npos &&
							fileInfo.getFileNumber() == 1)),
						  "invalid signature xml found: " << fileInfo.getFileName() <<
						  " with number: " << fileInfo.getFileNumber());
}

RUNNER_TEST(T00102_positive_public_check_ref)
{
	auto retVector = runCheck(TestData::widget_path,
							  true,
							  true);

	for (const auto &ret : retVector) {
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: " << ret.first);

		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
			RUNNER_ASSERT_MSG(sigData.getVisibilityLevel() == CertStoreId::VIS_PUBLIC,
							  "visibility check failed.");
	}
}

RUNNER_TEST(T00103_positive_partner_check_ref)
{
	auto retVector = runCheck(TestData::widget_partner_path,
							  true,
							  true);

	for (const auto &ret : retVector) {
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: " << ret.first);

		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
			RUNNER_ASSERT_MSG(sigData.getVisibilityLevel() == CertStoreId::VIS_PARTNER,
							  "visibility check failed.");
	}
}

RUNNER_TEST(T00104_positive_public_uncheck_ref)
{
	auto retVector = runCheck(TestData::widget_path,
							  true,
							  false);

	for (const auto &ret : retVector) {
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: " << ret.first);

		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
			RUNNER_ASSERT_MSG(sigData.getVisibilityLevel() == CertStoreId::VIS_PUBLIC,
							  "visibility check failed.");
	}
}

RUNNER_TEST(T00105_positive_partner_uncheck_ref)
{
	auto retVector = runCheck(TestData::widget_partner_path,
							  true,
							  false);

	for (const auto &ret : retVector) {
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: " << ret.first);

		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
			RUNNER_ASSERT_MSG(sigData.getVisibilityLevel() == CertStoreId::VIS_PARTNER,
							  "visibility check failed.");
	}
}

RUNNER_TEST(T00106_positive_tpk)
{
	auto retVector = runCheck(TestData::tpk_path,
							  true,
							  true);

	for (const auto &ret : retVector)
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: " << ret.first);
}

RUNNER_TEST(T00107_positive_tpk_with_userdata)
{
	UriList uriList;
	uriList.emplace_back("author-siganture.xml");
	uriList.emplace_back("bin/preference");
	uriList.emplace_back("res/edje/pref_buttons_panel.edj");
	uriList.emplace_back("res/edje/pref_edit_panel.edj");
	uriList.emplace_back("res/edje/preference.edj");
	uriList.emplace_back("res/images/icon_delete.png");
	uriList.emplace_back("res/res.xml");
	uriList.emplace_back("shared/res/preference.png");
	uriList.emplace_back("tizen-manifest.xml");

	auto retVector = runCheckList(TestData::tpk_with_userdata_path,
								  true,
								  uriList);

	for (const auto &ret : retVector)
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: "
						  << ret.first);
}

RUNNER_TEST(T00108_distributor_disregard_check)
{
	auto retVector = runCheck(TestData::widget_dist22_path,
							  true,
							  true);

	for (const auto &ret : retVector) {
		auto sigData = ret.second;
		if (sigData.isAuthorSignature())
			RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_CHAIN,
							  "author sig validation should be fail : "
							  << ret.first);
		else if (sigData.getSignatureNumber() == 1)
			RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_CHAIN,
							  "dist1 sig validation should be fail: "
							  << ret.first);
		else
			RUNNER_ASSERT_MSG(ret.first == E_SIG_DISREGARDED,
							  "dist22 sig validation should be disregarded: "
							  << ret.first);
	}
}

RUNNER_TEST(T00109_positive_platform_check_ref)
{
	auto retVector = runCheck(TestData::widget_platform_path,
							  true,
							  true);

	for (const auto &ret : retVector) {
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: " << ret.first);

		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
			RUNNER_ASSERT_MSG(sigData.getVisibilityLevel() == CertStoreId::VIS_PLATFORM,
							  "visibility check failed.");
	}
}

RUNNER_TEST(T00110_positive_platform_uncheck_ref)
{
	auto retVector = runCheck(TestData::widget_platform_path,
							  true,
							  false);

	for (const auto &ret : retVector) {
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: " << ret.first);

		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
			RUNNER_ASSERT_MSG(sigData.getVisibilityLevel() == CertStoreId::VIS_PLATFORM,
							  "visibility check failed.");
	}
}

RUNNER_TEST(T00111_positive_wgt_link)
{
	auto retVector = runCheck(TestData::widget_positive_link_path,
							  true,
							  true);

	for (const auto &ret : retVector) {
		auto sigData = ret.second;
		// this condition is for OCSP Success in signature1.xml
		if (sigData.getSignatureNumber() == 1)
			RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
							  "If DT_LNK type point within package, "
							  "it should be success: "
							  << ret.first);
	}
}

RUNNER_TEST(T00112_negative_wgt_link)
{
	auto retVector = runCheck(TestData::widget_negative_link_path,
							  true,
							  true);

	for (const auto &ret : retVector) {
		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
			RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_REF,
							  "If DT_LNK type point outside of package, "
							  "it should be fail: "
							  << ret.first);
	}
}

RUNNER_TEST(T00151_negative_hash_check_ref)
{
	auto retVector = runCheck(TestData::widget_negative_hash_path,
							  true,
							  true);

	for (const auto &ret : retVector) {
		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
				RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_SIG,
								  "dist sig shouldn't be success: "
								  << ret.first);
	}
}

RUNNER_TEST(T00152_negative_hash_uncheck_ref)
{
	auto retVector = runCheck(TestData::widget_negative_hash_path,
							  true,
							  false);

	for (const auto &ret : retVector) {
		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
				RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_SIG,
								  "dist sig shouldn't be success: "
								  << ret.first);
	}
}

RUNNER_TEST(T00153_negative_signature_check_ref)
{
	auto retVector = runCheck(TestData::widget_negative_signature_path,
							  true,
							  true);

	for (const auto &ret : retVector) {
		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
				RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_SIG,
								  "dist sig shouldn't be success: "
								  << ret.first);
	}
}

RUNNER_TEST(T00154_negative_signature_uncheck_ref)
{
	auto retVector = runCheck(TestData::widget_negative_signature_path,
							  true,
							  false);

	for (const auto &ret : retVector) {
		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
				RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_SIG,
								  "dist sig shouldn't be success: "
								  << ret.first);
	}
}

RUNNER_TEST(T00155_negative_tpk_with_added_malfile)
{
	auto retVector = runCheck(TestData::attacked_tpk_path,
							  true,
							  true);

	for (const auto &ret : retVector) {
		auto sigData = ret.second;
		if (sigData.getSignatureNumber() == 1)
				RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_REF,
								  "dist sig shouldn't be success: "
								  << ret.first);
	}
}

RUNNER_TEST(T00156_negative_tpk_with_userdata_file_changed_in_list)
{
	UriList uriList;
	uriList.emplace_back("author-siganture.xml");
	uriList.emplace_back("bin/preference");
	uriList.emplace_back("res/edje/pref_buttons_panel.edj");
	uriList.emplace_back("res/edje/pref_edit_panel.edj");
	uriList.emplace_back("res/edje/preference.edj");
	uriList.emplace_back("res/images/icon_delete.png");
	uriList.emplace_back("shared/res/preference.png");
	/* this file is modified after signing app */
	uriList.emplace_back("res/res.xml");
	uriList.emplace_back("tizen-manifest.xml");

	auto retVector = runCheckList(TestData::attacked_tpk_with_userdata_path,
								  true,
								  uriList);

	for (const auto &ret : retVector)
		RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_SIG,
						  "sig validation should be E_SIG_INVALID_SIG: "
						  << ret.first);
}

RUNNER_TEST(T00157_negative_tpk_with_userdata_file_changed_in_list_2)
{
	UriList uriList;
	uriList.emplace_back("author-siganture.xml");
	uriList.emplace_back("bin/preference");
	uriList.emplace_back("res/edje/pref_buttons_panel.edj");
	uriList.emplace_back("res/edje/pref_edit_panel.edj");
	uriList.emplace_back("res/edje/preference.edj");
	uriList.emplace_back("res/images/icon_delete.png");
	uriList.emplace_back("shared/res/preference.png");
	/* this file is modified after signing app */
	uriList.emplace_back("res/res.xml");
	/* force disable below for only checking above one */
	// uriList.emplace_back("tizen-manifest.xml");

	auto retVector = runCheckList(TestData::attacked_tpk_with_userdata_path,
								  true,
								  uriList);

	for (const auto &ret : retVector)
		RUNNER_ASSERT_MSG(ret.first == E_SIG_INVALID_SIG,
						  "sig validation should be E_SIG_INVALID_SIG: "
						  << ret.first);
}

RUNNER_TEST(T00158_negative_tpk_with_userdata_file_changed_in_list_3)
{
	UriList uriList;
	uriList.emplace_back("res/edje/pref_buttons_panel.edj");
	uriList.emplace_back("res/images/icon_delete.png");
	uriList.emplace_back("shared/res/preference.png");
	/* this file is modified after signing app */
	// uriList.emplace_back("res/res.xml");
	// uriList.emplace_back("tizen-manifest.xml");

	auto retVector = runCheckList(TestData::attacked_tpk_with_userdata_path,
								  true,
								  uriList);

	for (const auto &ret : retVector)
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: "
						  << ret.first);
}

RUNNER_TEST(T00159_negative_tpk_with_nohash)
{
	UriList uriList;
	auto retVector = runCheckList(TestData::attacked_tpk_with_userdata_path,
								  true,
								  uriList);

	for (const auto &ret : retVector)
		RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
						  "sig validation should be success: "
						  << ret.first);
}

RUNNER_TEST(T00160_positive_checkAll)
{
	SignatureValidator validator(TestData::widget_path);
	SignatureDataMap sigDataMap;
	VCerr result = validator.checkAll(true, true, sigDataMap);

	RUNNER_ASSERT_MSG(result == E_SIG_NONE,
					  "sig validation should be success: "
					  << validator.errorToString(result));

	/* Below codes is example for client.
	 *
	auto authorSigData = sigDataMap[ValidationCore::SignatureType::AUTHOR];
	for (const auto &certPtr : authorSigData.getCertList())
		std::cout << "Author certificate > " << certPtr->getBase64() << std::endl;

	auto distSigData = sigDataMap[ValidationCore::SignatureType::DISTRIBUTOR1];
	for (const auto &certPtr : distSigData.getCertList())
		std::cout << "Distributor certificate > "
				  << certPtr->getBase64() << std::endl;
	 *
	 */
}

RUNNER_TEST(T00161_positive_checkListAll)
{
	SignatureValidator validator(TestData::tpk_with_userdata_path);
	UriList uriList;
	uriList.emplace_back("author-siganture.xml");
	uriList.emplace_back("bin/preference");
	uriList.emplace_back("res/edje/pref_buttons_panel.edj");
	uriList.emplace_back("res/edje/pref_edit_panel.edj");
	uriList.emplace_back("res/edje/preference.edj");
	uriList.emplace_back("res/images/icon_delete.png");
	uriList.emplace_back("res/res.xml");
	uriList.emplace_back("shared/res/preference.png");
	uriList.emplace_back("tizen-manifest.xml");

	SignatureDataMap sigDataMap;
	VCerr result = validator.checkListAll(true, uriList, sigDataMap);
	RUNNER_ASSERT_MSG(result == E_SIG_NONE,
					  "sig validation should be success: "
					  << validator.errorToString(result));

	/* Below codes is example for client.
	 *
	auto authorSigData = sigDataMap[ValidationCore::SignatureType::AUTHOR];
	for (const auto &certPtr : authorSigData.getCertList())
		std::cout << "Author certificate > " << certPtr->getBase64() << std::endl;

	auto distSigData = sigDataMap[ValidationCore::SignatureType::DISTRIBUTOR1];
	for (const auto &certPtr : distSigData.getCertList())
		std::cout << "Distributor certificate > "
				  << certPtr->getBase64() << std::endl;
	 *
	 */
}

RUNNER_TEST(T00162_compare_time_between_check_and_checkAll)
{
	for(int i = 0; i < 3; i++) {
		std::cout << "Start to validate : "
				  << TestData::tpk_sdk_sample_path[i] << std::endl;

		Test::cmpFuncTime(
			[&]() { // func1
				auto retVector = runCheck(TestData::tpk_sdk_sample_path[i],
										  true,
										  true);

				for (auto &ret : retVector)
					RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
									  "sig validation should be success: "
									  << ret.first);
			},
			[&]() { // func2
				auto ret = runCheckAll(TestData::tpk_sdk_sample_path[i],
									   true,
									   true);

				RUNNER_ASSERT_MSG(ret == E_SIG_NONE,
								  "sig validation should be success: "
								  << ret);
			});
	}
}

RUNNER_TEST(T00163_compare_time_between_checkList_and_checkListAll)
{
	UriList uriList;
	uriList.emplace_back("bin/player");
	uriList.emplace_back("res/sample.3gp");
	uriList.emplace_back("res/test0.3gp");
	uriList.emplace_back("res/test1.wav");
	uriList.emplace_back("res/test2.wav");

	std::cout << "Start to validate : "
			  << TestData::tpk_sdk_sample_path[2] << std::endl;

	Test::cmpFuncTime(
		[&]() { // func1
			auto retVector = runCheckList(TestData::tpk_sdk_sample_path[2],
										  true,
										  uriList);

			for (const auto &ret : retVector)
				RUNNER_ASSERT_MSG(ret.first == E_SIG_NONE,
								  "sig validation should be success: "
								  << ret.first);
		},
		[&]() { // func2
			auto ret = runCheckListAll(TestData::tpk_sdk_sample_path[2],
									   true,
									   uriList);

			RUNNER_ASSERT_MSG(ret == E_SIG_NONE,
							  "sig validation should be success: "
							  << ret);
		});
}

RUNNER_TEST_GROUP_INIT(T0020_SigVal_errorstring)

RUNNER_TEST(T0021)
{
	SignatureValidator validator(SignatureFileInfo("test-dummy", 1));

	for (VCerr code = E_SCOPE_FIRST; code >= E_SCOPE_LAST; code--) {
		std::cout << "E_SIG code["
				  << code << "] : "
				  << validator.errorToString(code) << std::endl;
	}

	/* print 10 more error code below last in case of plugin err exist */
	for (VCerr code = E_SCOPE_LAST - 1; code >= E_SCOPE_LAST - 10; code--) {
		std::cout << "VCerr from plugin["
				  << code << "] : "
				  << validator.errorToString(code) << std::endl;
	}
}
