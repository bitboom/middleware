/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file        certificate.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "certificate.hxx"

#include <cstdio>
#include <memory>
#include <vector>

#include "file-system.hxx"
#include "exception.hxx"

#include <openssl/pem.h>

namespace tanchor {

namespace {

using X509Ptr = std::unique_ptr<X509, decltype(&::X509_free)>;

const std::string START_CERT    = "-----BEGIN CERTIFICATE-----";
const std::string END_CERT      = "-----END CERTIFICATE-----";
const std::string START_TRUSTED = "-----BEGIN TRUSTED CERTIFICATE-----";
const std::string END_TRUSTED   = "-----END TRUSTED CERTIFICATE-----";

const int HASH_LENGTH = 8;

} // namespace anonymous

Certificate::Certificate(const std::string &path) : m_path(path)
{
}

std::string Certificate::getSubjectNameHash() const
{
	FilePtr fp = FilePtr(::fopen(this->m_path.c_str(), "rb"), ::fclose);
	if (fp == nullptr)
		throw std::invalid_argument("Failed to open [" + this->m_path + "].");

	X509Ptr x509(::PEM_read_X509(fp.get(), NULL, NULL, NULL),
				 ::X509_free);
	if (x509 == nullptr) {
		::rewind(fp.get());
		x509 = X509Ptr(::PEM_read_X509_AUX(fp.get(), NULL, NULL, NULL),
					   ::X509_free);
	}

	if (x509 == nullptr)
		throw std::logic_error("Failed to read certificate.");

	std::vector<char> buf(HASH_LENGTH + 1);
	snprintf(buf.data(), buf.size(),
			 "%08lx", ::X509_subject_name_hash(x509.get()));

	return std::string(buf.data(), HASH_LENGTH);
}

std::string Certificate::getCertificateData() const
{
	std::string content = File::read(this->m_path);
	return this->parseData(content);
}

std::string Certificate::parseData(const std::string &data) const
{
	if (data.empty())
		throw std::logic_error("There is no data to parse.");

	size_t from = data.find(START_CERT);
	size_t to = data.find(END_CERT);
	size_t tailLen = END_CERT.length();

	if (from == std::string::npos || to == std::string::npos || from > to) {
		from = data.find(START_TRUSTED);
		to = data.find(END_TRUSTED);
		tailLen = END_TRUSTED.length();
	}

	if (from == std::string::npos || to == std::string::npos || from > to)
		throw std::logic_error("Failed to parse certificate.");

	return std::string(data, from, to - from + tailLen);
}

} // namespace tanchor
