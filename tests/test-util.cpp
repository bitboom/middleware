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
 * @file        test-util.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief
 */
#include "test-util.hxx"

#include <sched.h>

#include <cstdio>
#include <memory>
#include <vector>

#include <curl/curl.h>

namespace test {
namespace util {

std::string ls(const char *path)
{
	using FilePtr = std::unique_ptr<FILE, decltype(&::pclose)>;
	std::string cmd("/bin/ls ");
	cmd.append(path);

	FilePtr ls(::popen(cmd.c_str(), "r"), ::pclose);
	if (ls == nullptr)
		return std::string();

	std::vector<char> buf(1024);
	std::string ret;
	while (::fgets(buf.data(), buf.size(), ls.get()))
		ret.append(buf.data());

	return ret;
}

std::string cat(const char *path)
{
	using FilePtr = std::unique_ptr<FILE, decltype(&::pclose)>;
	std::string cmd("/bin/cat ");
	cmd.append(path);

	FilePtr ls(::popen(cmd.c_str(), "r"), ::pclose);
	if (ls == nullptr)
		return std::string();

	std::vector<char> buf(1024);
	std::string ret;
	while (::fgets(buf.data(), buf.size(), ls.get()))
		ret.append(buf.data());

	return ret;
}

int connectSSL(const std::string &addr)
{
	if (addr.empty())
		return -1;

	using CURLPtr = std::unique_ptr<CURL, decltype(&::curl_easy_cleanup)>;
	CURLPtr curl(::curl_easy_init(), ::curl_easy_cleanup);
	if (curl == nullptr) {
		std::cout << "Failed to get curl object." << std::endl;
		return -1;
	}

	curl_easy_setopt(curl.get(), CURLOPT_URL, addr.c_str());

	CURLcode res = curl_easy_perform(curl.get());
	if (res != CURLE_OK) {
		std::cout << "Failed to connect failed: "
				  << curl_easy_strerror(res) << std::endl;
		return -1;
	}

	return 0;
}

} // namespace util
} // namespace test
