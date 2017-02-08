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
 * @file        Certificate.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief
 */
#include "Certificate.h"

#include <cstdio>
#include <vector>
#include <memory>
#include <stdexcept>

#include <openssl/pem.h>

namespace transec {

namespace {

using FilePtr = std::unique_ptr<FILE, decltype(&::fclose)>;
using X509Ptr = std::unique_ptr<X509, decltype(&::X509_free)>;

const int HASH_LENGTH = 8;

} // namespace anonymous

std::string Certificate::getSubjectNameHash(const std::string &path)
{
	FilePtr fp(fopen(path.c_str(), "r"), ::fclose);
	if (fp == nullptr)
		throw std::invalid_argument("Faild to open certificate.");

	X509Ptr x509(::PEM_read_X509(fp.get(), NULL, NULL, NULL), ::X509_free);
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

} // namespace transec
