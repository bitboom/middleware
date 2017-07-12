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
 * @file        file-system.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "file-system.hxx"

#include <climits>
#include <cerrno>
#include <unistd.h>
#include <cstdio>

#include <vector>

#include "exception.hxx"

namespace tanchor {

void File::linkTo(const std::string &src, const std::string &dst)
{
	if (::symlink(src.c_str(), dst.c_str()))
		ThrowErrno(errno, "Failed to link " + src + " -> " + dst);
}

std::string File::read(const std::string &path)
{
	FilePtr fp = FilePtr(::fopen(path.c_str(), "rb"), ::fclose);
	if (fp == nullptr)
		throw std::invalid_argument("Failed to open [" + path + "].");

	std::fseek(fp.get(), 0L, SEEK_END);
	unsigned int fsize = std::ftell(fp.get());
	std::rewind(fp.get());

	std::string buff(fsize, 0);
	if (fsize != std::fread(static_cast<void*>(&buff[0]), 1, fsize, fp.get()))
		throw std::logic_error("Failed to read [" + path + "]");
	return buff;
}

std::string File::readLink(const std::string &path)
{
	std::vector<char> buf(PATH_MAX);
	ssize_t count = ::readlink(path.c_str(), buf.data(), buf.size());
	return std::string(buf.data(), (count > 0) ? count : 0);
}

std::string File::getName(const std::string &path)
{
	size_t pos = path.rfind('/');
	if (pos == std::string::npos)
		throw std::invalid_argument("Path is wrong. > " + path);
	return path.substr(pos + 1);
}

} // namespace tanchor
