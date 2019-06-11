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
#include <generic-backend/ipassword.h>
#include <generic-backend/password-file-buffer.h>

#include <error-description.h>
#include <password-exception.h>

#include <dpl/fstream_accessors.h>

#include <openssl/sha.h>

namespace AuthPasswd {

NoPassword::NoPassword(IStream&)
{
}

void NoPassword::Serialize(IStream &stream) const
{
	Serialization::Serialize(stream, static_cast<unsigned int>(PasswordType::NONE));
}

bool NoPassword::match(const std::string &pass) const
{
	return pass.empty();
}

SHA256Password::SHA256Password(IStream &stream)
{
	Deserialization::Deserialize(stream, m_hash);
}

SHA256Password::SHA256Password(const std::string &password) : m_hash(hash(password))
{
}

SHA256Password::SHA256Password(const RawHash &paramHash) : m_hash(paramHash)
{
}

void SHA256Password::Serialize(IStream &stream) const
{
	Serialization::Serialize(stream, static_cast<unsigned int>(PasswordType::SHA256));
	Serialization::Serialize(stream, m_hash);
}

bool SHA256Password::match(const std::string &password) const
{
	return m_hash == hash(password);
}

RawHash SHA256Password::hash(const std::string &password)
{
	RawHash result(SHA256_DIGEST_LENGTH);
	SHA256_CTX context;
	SHA256_Init(&context);
	SHA256_Update(&context, reinterpret_cast<const unsigned char *>(password.c_str()),
				  password.size());
	SHA256_Final(result.data(), &context);
	return result;
}

template <>
void Deserialization::Deserialize(IStream &stream, IPasswordPtr &ptr)
{
	unsigned int algorithm;
	Deserialization::Deserialize(stream, algorithm);

	switch (algorithm) {
	case (unsigned int)IPassword::PasswordType::NONE:
		ptr.reset(new NoPassword());
		break;

	case (unsigned int)IPassword::PasswordType::SHA256:
		ptr.reset(new SHA256Password(stream));
		break;

	default:
		Throw(PasswordException::FStreamReadError);
	}
}

} //namespace AuthPasswd
