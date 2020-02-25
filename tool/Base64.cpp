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

namespace base64
{

static char bytesToChar[] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
	'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
	't', 'u', 'v', 'w', 'x', 'y', 'z',

	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

static_assert(sizeof(bytesToChar) == 64, "Invalid bytes to char mapping.");

std::string encode(const void* data, std::size_t size)
{
	std::string result;
	if (!data || size == 0)
		return result;

	auto bytes = reinterpret_cast<const std::uint8_t*>(data);
	for (; size >= 3; bytes += 3, size -= 3)
	{
		std::uint32_t triplet = (bytes[0] << 16) | (bytes[1] << 8) | (bytes[2]);
		result.push_back(bytesToChar[triplet >> 18]);
		result.push_back(bytesToChar[(triplet >> 12) & 0x3F]);
		result.push_back(bytesToChar[(triplet >> 6) & 0x3F]);
		result.push_back(bytesToChar[triplet & 0x3F]);
	}

	switch (size)
	{
		case 2:
		{
			std::uint32_t duo = (bytes[0] << 8) | bytes[1];
			result.push_back(bytesToChar[duo >> 10]);
			result.push_back(bytesToChar[(duo >> 4) & 0x3F]);
			result.push_back(bytesToChar[(duo << 2) & 0x3F]);
			result.push_back('=');
			break;
		}
		case 1:
		{
			std::uint32_t single = *bytes;
			result.push_back(bytesToChar[single >> 2]);
			result.push_back(bytesToChar[(single << 4) & 0x3F]);
			result.push_back('=');
			result.push_back('=');
			break;
		}
	}

	return result;
}

bool decode(std::vector<std::uint8_t>& outData, const char* encoded)
{
	if (!encoded)
		return false;

	while (*encoded)
	{
		std::uint32_t triplet = 0;
		unsigned int byteCount = 3;
		for (int i = 0; i < 4; ++i, ++encoded)
		{
			char c = *encoded;
			std::uint32_t index;
			if (c >= 'A' && c <= 'Z')
				index = c - 'A';
			else if (c >= 'a' && c <= 'z')
				index = c - 'a' + 26;
			else if (c >= '0' && c <= '9')
				index = c - '0' + 52;
			else if (c == '+')
				index = 62;
			else if (c == '/')
				index = 63;
			else if (c == '=')
			{
				// Either ends with one or two =, otherwise not valid.
				if (i < 2 || (i == 2 && encoded[1] != '='))
					return false;
				--byteCount;
				continue;
			}
			else
				return false;

			triplet |= index << ((3 - i)*6);
		}

		// Only allow = at end.
		if (byteCount != 3 && *encoded)
			return false;

		for (unsigned int i = 0; i < byteCount; ++i)
			outData.push_back(static_cast<std::uint8_t>((triplet >> ((2 - i)*8)) & 0xFF));
	}

	return true;
}

} // base64
