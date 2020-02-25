/*
 * Copyright 2020 Aaron Barany
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Base64.h"
#include <cstring>
#include <gtest/gtest.h>

TEST(Base64Test, AllCharacters)
{
	const char* encoded = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::vector<std::uint8_t> data;
	ASSERT_TRUE(base64::decode(data, encoded));
	EXPECT_EQ(encoded, base64::encode(data.data(), data.size()));
}

TEST(Base64Test, Padding)
{
	EXPECT_EQ("TWFu", base64::encode("Man", 3));
	EXPECT_EQ("TWE=", base64::encode("Ma", 2));
	EXPECT_EQ("TQ==", base64::encode("M", 1));

	std::vector<std::uint8_t> data;
	ASSERT_TRUE(base64::decode(data, "TWFu"));
	EXPECT_EQ(std::vector<std::uint8_t>({'M', 'a', 'n'}), data);

	data.clear();
	ASSERT_TRUE(base64::decode(data, "TWE="));
	EXPECT_EQ(std::vector<std::uint8_t>({'M', 'a'}), data);

	data.clear();
	ASSERT_TRUE(base64::decode(data, "TQ=="));
	EXPECT_EQ(std::vector<std::uint8_t>({'M'}), data);
}

TEST(Base64Test, EncodedString)
{
	// Known example from https://en.wikipedia.org/wiki/Base64
	// Encoding doesn't include NUL terminator.
	const char* message = "Man is distinguished, not only by his reason, but by this singular "
		"passion from other animals, which is a lust of the mind, that by a perseverance of "
		"delight in the continued and indefatigable generation of knowledge, exceeds the short "
		"vehemence of any carnal pleasure.";
	std::size_t length = std::strlen(message);
	const char* encodedMessage = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1d"
		"CBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGh"
		"lIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZ"
		"mF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW5"
		"5IGNhcm5hbCBwbGVhc3VyZS4=";
	EXPECT_EQ(encodedMessage, base64::encode(message, length));

	std::vector<std::uint8_t> data;
	ASSERT_TRUE(base64::decode(data, encodedMessage));
	std::string decodedMessage(data.begin(), data.end());
	EXPECT_EQ(message, decodedMessage);
}

TEST(Base64Test, DecodeError)
{
	std::vector<std::uint8_t> data;
	EXPECT_FALSE(base64::decode(data, "AB"));
	EXPECT_FALSE(base64::decode(data, "ABCDABC"));
	EXPECT_FALSE(base64::decode(data, "ABC("));
	EXPECT_FALSE(base64::decode(data, "AB=D"));
	EXPECT_FALSE(base64::decode(data, "ABC=ABCD"));
	EXPECT_FALSE(base64::decode(data, "A==="));
}
