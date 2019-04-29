/*
 * Copyright (c) 2016-2019 Samsung Electronics Co., Ltd. All rights reserved
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
 * @file        ReferenceValidator.cpp
 * @version     1.0
 * @brief       Compare signature reference list and list of widget file.
 */
#include <vcore/ReferenceValidator.h>

#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <memory>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pcrecpp.h>

#include <dpl/log/log.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

namespace ValidationCore {

namespace {

const char *AUTHOR_SIGNATURE = "author-signature.xml";
const char *REGEXP_DISTRIBUTOR_SIGNATURE = "^signature[1-9][0-9]*\\.xml";
const char MARK_ENCODED_CHAR = '%';

struct dirent *readdir(DIR *dirp) {
	errno = 0;
	auto ret = ::readdir(dirp);
	if (errno != 0)
		LogWarning("Error read dir.");
	return ret;
}

} // anonymous namespace

class ReferenceValidator::Impl {
public:
	Impl(const std::string &dirpath)
		: m_dirpath(dirpath)
		, m_signatureRegexp(REGEXP_DISTRIBUTOR_SIGNATURE)
	{}

	virtual ~Impl() {}

	Result checkReferences(const SignatureData &signatureData)
	{
		const ReferenceSet &refSet = signatureData.getReferenceSet();
		ReferenceSet refDecoded;

		try {
			for (auto it = refSet.begin(); it != refSet.end(); ++it) {
				if (std::string::npos != it->find(MARK_ENCODED_CHAR))
					refDecoded.insert(decodeProcent(*it));
				else
					refDecoded.insert(*it);
			}
		} catch (Result &) {
			return ERROR_DECODING_URL;
		}

		return dfsCheckDirectories(
				   refDecoded,
				   std::string(),
				   signatureData.isAuthorSignature());
	}

	Result checkOutbound(const std::string &linkPath, const std::string &appPath)
	{
		const auto path = appPath + "/" + linkPath;
		const auto resolvedPath = realpath(path.c_str(), nullptr);
		if (!resolvedPath)
			return ERROR_READING_LNK;
		const auto ret =
			strncmp(resolvedPath, appPath.c_str(), appPath.size())
			|| resolvedPath[appPath.size()] != '/'
				? ERROR_OUTBOUND_LNK
				: NO_ERROR;
		free(resolvedPath);
		return ret;
	}

private:
	int hexToInt(char hex);
	std::string decodeProcent(const std::string &path);

	Result dfsCheckDirectories(
		const ReferenceSet &referenceSet,
		const std::string &directory,
		bool isAuthorSignature);

	inline bool isDistributorSignature(const char *cstring) const
	{
		return m_signatureRegexp.FullMatch(cstring);
	}

	std::string m_dirpath;
	pcrecpp::RE m_signatureRegexp;
};

int ReferenceValidator::Impl::hexToInt(char a)
{
	if (a >= '0' && a <= '9') return a - '0';

	if (a >= 'A' && a <= 'F') return a - 'A' + 10;

	if (a >= 'a' && a <= 'f') return a - 'a' + 10;

	LogError("Symbol '" << a << "' is out of scope.");
	throw ERROR_DECODING_URL;
}

std::string ReferenceValidator::Impl::decodeProcent(const std::string &path)
{
	std::vector<int> input(path.begin(), path.end());
	std::vector<char> output;

	try {
		size_t i = 0;

		while (i < input.size()) {
			if (MARK_ENCODED_CHAR == input[i]) {
				if (i + 2 >= input.size())
					throw ERROR_DECODING_URL;

				int result = hexToInt(input[i + 1]) * 16 + hexToInt(input[i + 2]);
				output.push_back(static_cast<char>(result));
				i += 3;
			} else {
				output.push_back(static_cast<char>(input[i]));
				++i;
			}
		}
	} catch (Result &) {
		LogError("Error while decoding url path : " << path);
		throw ERROR_DECODING_URL;
	}

	return std::string(output.begin(), output.end());
}

ReferenceValidator::Result ReferenceValidator::Impl::dfsCheckDirectories(
	const ReferenceSet &referenceSet,
	const std::string &directory,
	bool isAuthorSignature)
{
	std::string currentDir = m_dirpath;
	if (!directory.empty())
		currentDir += ("/" + directory);

	std::unique_ptr<DIR, std::function<int(DIR *)>> dp(::opendir(currentDir.c_str()),
													   ::closedir);
	if (dp == nullptr) {
		LogError("Error opening directory : " << currentDir);
		return ERROR_OPENING_DIR;
	}

	while (auto dirp = ValidationCore::readdir(dp.get())) {
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		if (dirp->d_type == DT_UNKNOWN) {
			// try to stat inode when readdir is not returning known type
			std::string path = currentDir + "/" + dirp->d_name;
			struct stat s;
			if (lstat(path.c_str(), &s) != 0)
				return ERROR_LSTAT;

			if (S_ISREG(s.st_mode))
				dirp->d_type = DT_REG;
			else if (S_ISDIR(s.st_mode))
				dirp->d_type = DT_DIR;
		}

		if (currentDir == m_dirpath && dirp->d_type == DT_REG)
			if ((!strcmp(dirp->d_name, AUTHOR_SIGNATURE) && isAuthorSignature) ||
				isDistributorSignature(dirp->d_name))
				continue;

		if (dirp->d_type == DT_DIR) {
			LogDebug("Open directory : " << (directory + dirp->d_name));
			std::string tmp_directory = directory + dirp->d_name + "/";
			Result result = dfsCheckDirectories(referenceSet,
												tmp_directory,
												isAuthorSignature);
			if (result != NO_ERROR)
				return result;
		} else if (dirp->d_type == DT_REG) {
			if (referenceSet.end() == referenceSet.find(directory + dirp->d_name)) {
				LogError("Cannot find : " << (directory + dirp->d_name));
				return ERROR_REFERENCE_NOT_FOUND;
			}
		} else if (dirp->d_type == DT_LNK) {
			std::string linkPath(directory + dirp->d_name);
			if (referenceSet.end() == referenceSet.find(linkPath)) {
				LogError("Cannot find : " << (directory + dirp->d_name));
				return ERROR_REFERENCE_NOT_FOUND;
			}

			Result result = checkOutbound(linkPath, m_dirpath);
			if (result != NO_ERROR) {
				LogError("Link file point wrong path. : " << linkPath);
				return result;
			}
		} else {
			LogError("Unknown file type.");
			return ERROR_UNSUPPORTED_FILE_TYPE;
		}
	}



	return NO_ERROR;
}

ReferenceValidator::ReferenceValidator(const std::string &dirpath)
	: m_impl(new Impl(dirpath))
{}

ReferenceValidator::~ReferenceValidator()
{
	delete m_impl;
}

ReferenceValidator::Result ReferenceValidator::checkReferences(
	const SignatureData &signatureData)
{
	return m_impl->checkReferences(signatureData);
}

ReferenceValidator::Result ReferenceValidator::checkOutbound(
	const std::string &linkPath, const std::string &appPath)
{
	return m_impl->checkOutbound(linkPath, appPath);
}
} // ValidationCore
