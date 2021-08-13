/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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
#ifndef _IPASSWORD_H_
#define _IPASSWORD_H_

#include <string>
#include <memory>
#include <limits>

#include <time.h>

#include <dpl/serialization.h>

namespace AuthPasswd {

constexpr time_t PASSWORD_INFINITE_EXPIRATION_TIME = std::numeric_limits<time_t>::max();

struct IPassword;
using RawHash = std::vector<unsigned char>;
using IPasswordPtr = std::shared_ptr<IPassword>;
using PasswordList = std::list<IPasswordPtr>;

struct IPassword: public ISerializable {

	enum class PasswordType : unsigned int {
		NONE = 0,
		SHA256 = 1,
	};

	virtual bool match(const std::string &password) const = 0;
};

class NoPassword: public IPassword {
public:
	NoPassword() = default;
	NoPassword(IStream &);

	void Serialize(IStream &stream) const;
	bool match(const std::string &pass) const;
};

class SHA256Password: public IPassword {
public:
	SHA256Password(IStream &stream);
	SHA256Password(const std::string &password);
	SHA256Password(const RawHash &paramHash);

	void Serialize(IStream &stream) const;
	bool match(const std::string &password) const;

private:
	static RawHash hash(const std::string &password);

	RawHash m_hash;
};

template <>
void Deserialization::Deserialize(IStream &stream, IPasswordPtr &ptr);

} //namespace AuthPasswd

#endif
