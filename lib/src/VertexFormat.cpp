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

#include <VFC/VertexFormat.h>

#include <cassert>
#include <cctype>
#include <cstdint>
#include <unordered_map>
#include <utility>

#include <string.h>

#if VFC_WINDOWS
#define strcasecmp(x, y) _stricmp(x, y)
#else
#include <strings.h>
#endif

namespace vfc
{

namespace
{

class StringRef
{
public:
	StringRef(const char* str)
		: m_str(str)
	{
	}

	bool operator==(StringRef other) const
	{
		return strcasecmp(m_str, other.m_str) == 0;
	}

	bool operator!=(StringRef other) const
	{
		return strcasecmp(m_str, other.m_str) != 0;
	}

	std::size_t hash() const
	{
		std::size_t value = 5381;
		const char* str = m_str;
		while (int c = *str++)
		{
			c = std::tolower(c);
			value = ((value << 5) + value) + c; // value*33 + c
		}
		return value;
	}

private:
	const char* m_str;
};

struct StringRefHash
{
	std::size_t operator()(StringRef str) const
	{
		return str.hash();
	}
};

const char* const elementLayoutNames[] =
{
	"X8",
	"X8Y8",
	"X8Y8Z8",
	"X8Y8Z8W8",
	"W2X10Y10Z10",
	"W2Z10Y10X10",
	"X16",
	"X16Y16",
	"X16Y16Z16",
	"X16Y16Z16W16",
	"X32",
	"X32Y32",
	"X32Y32Z32",
	"X32Y32Z32W32",
	"X64",
	"X64Y64",
	"X64Y64Z64",
	"X64Y64Z64W64",
	"Z10Y11X11_UFloat",
	"E5Z9Y9X9_UFloat"
};

static_assert(sizeof(elementLayoutNames)/sizeof(*elementLayoutNames) == elementLayoutCount,
	"Unexpected elementLayoutNames size.");

const char* const colorElementLayoutNames[] =
{
	"R8",
	"R8G8",
	"R8G8B8",
	"R8G8B8A8",
	"A2R10G10B10",
	"A2B10G10R10",
	"R16",
	"R16G16",
	"R16G16B16",
	"R16G16B16A16",
	"R32",
	"R32G32",
	"R32G32B32",
	"R32G32B32A32",
	"R64",
	"R64G64",
	"R64G64B64",
	"R64G64B64A64",
	"B10G11R11_UFloat",
	"E5B9G9R9_UFloat"
};

static_assert(
	sizeof(colorElementLayoutNames)/sizeof(*colorElementLayoutNames) == elementLayoutCount,
	"Unexpected colorElementLayoutNames size.");

const std::size_t elementLayoutSizes[] =
{
	sizeof(std::uint8_t),	 // X8
	sizeof(std::uint8_t)*2,	 // X8Y8
	sizeof(std::uint8_t)*3,	 // X8Y8Z8
	sizeof(std::uint8_t)*4,	 // X8Y8Z8W8
	sizeof(std::uint32_t),	 // W2X10Y10Z1,
	sizeof(std::uint32_t),	 // W2Z10Y10X10
	sizeof(std::uint16_t),	 // X16
	sizeof(std::uint16_t)*2, // X16Y16
	sizeof(std::uint16_t)*3, // X16Y16Z16
	sizeof(std::uint16_t)*4, // X16Y16Z16W16
	sizeof(std::uint32_t),	 // X32
	sizeof(std::uint32_t)*2, // X32Y32
	sizeof(std::uint32_t)*3, // X32Y32Z32
	sizeof(std::uint32_t)*4, // X32Y32Z32W32
	sizeof(std::uint64_t),	 // X64
	sizeof(std::uint64_t)*2, // X64Y64
	sizeof(std::uint64_t)*3, // X64Y64Z64
	sizeof(std::uint64_t)*4, // X64Y64Z64W64
	sizeof(std::uint32_t),   // Z10Y11X11_UFloat
	sizeof(std::uint32_t)    // E5Z9Y9X9_UFloat
};

static_assert(sizeof(elementLayoutSizes)/sizeof(*elementLayoutSizes) == elementLayoutCount,
	"Unexpected elementLayoutSizes size.");

const char* const elementTypeNames[] =
{
	"UNorm",
	"SNorm",
	"UInt",
	"SInt",
	"Float"
};

static_assert(sizeof(elementTypeNames)/sizeof(*elementTypeNames) == elementTypeCount,
	"Unexpected elementTypeNames size.");

const std::unordered_map<StringRef, ElementLayout, StringRefHash> elementLayoutMap =
{
	{"X8", ElementLayout::X8},
	{"X8Y8", ElementLayout::X8Y8},
	{"X8Y8Z8", ElementLayout::X8Y8Z8},
	{"X8Y8Z8W8", ElementLayout::X8Y8Z8W8},
	{"W2X10Y10Z10", ElementLayout::W2X10Y10Z10},
	{"W2Z10Y10X10", ElementLayout::W2Z10Y10X10},
	{"X16", ElementLayout::X16},
	{"X16Y16", ElementLayout::X16Y16},
	{"X16Y16Z16", ElementLayout::X16Y16Z16},
	{"X16Y16Z16W16", ElementLayout::X16Y16Z16W16},
	{"X32", ElementLayout::X32},
	{"X32Y32", ElementLayout::X32Y32},
	{"X32Y32Z32", ElementLayout::X32Y32Z32},
	{"X32Y32Z32W32", ElementLayout::X32Y32Z32W32},
	{"X64", ElementLayout::X64},
	{"X64Y64", ElementLayout::X64Y64},
	{"X64Y64Z64", ElementLayout::X64Y64Z64},
	{"X64Y64Z64W64", ElementLayout::X64Y64Z64W64},
	{"Z10Y11X11_UFloat", ElementLayout::Z10Y11X11_UFloat},
	{"E5Z9Y9X9_UFloat", ElementLayout::E5Z9Y9X9_UFloat},

	{"R8", ElementLayout::R8},
	{"R8G8", ElementLayout::R8G8},
	{"R8G8B8", ElementLayout::R8G8B8},
	{"R8G8B8A8", ElementLayout::R8G8B8A8},
	{"A2R10G10B10", ElementLayout::A2R10G10B10},
	{"A2B10G10R10", ElementLayout::A2B10G10R10},
	{"R16", ElementLayout::R16},
	{"R16G16", ElementLayout::R16G16},
	{"R16G16B16", ElementLayout::R16G16B16},
	{"R16G16B16A16", ElementLayout::R16G16B16A16},
	{"R32", ElementLayout::R32},
	{"R32G32", ElementLayout::R32G32},
	{"R32G32B32", ElementLayout::R32G32B32},
	{"R32G32B32A32", ElementLayout::R32G32B32A32},
	{"R64", ElementLayout::R64},
	{"R64G64", ElementLayout::R64G64},
	{"R64G64B64", ElementLayout::R64G64B64},
	{"R64G64B64A64", ElementLayout::R64G64B64A64},
	{"B10G11R11_UFloat", ElementLayout::B10G11R11_UFloat},
	{"E5B9G9R9_UFloat", ElementLayout::E5B9G9R9_UFloat}
};

const std::unordered_map<StringRef, ElementType, StringRefHash> elementTypeMap =
{
	{"UNorm", ElementType::UNorm},
	{"SNorm", ElementType::SNorm},
	{"UInt", ElementType::UInt},
	{"SInt", ElementType::SInt},
	{"Float", ElementType::Float}
};

} // namespace

const char* elementLayoutName(ElementLayout layout, bool color)
{
	auto index = static_cast<unsigned int>(layout);
	if (index >= elementLayoutCount)
		return nullptr;

	return color ? colorElementLayoutNames[index] : elementLayoutNames[index];
}

const char* elementTypeName(ElementType type)
{
	auto index = static_cast<unsigned int>(type);
	if (index >= elementTypeCount)
		return nullptr;

	return elementTypeNames[index];
}

ElementLayout elementLayoutFromName(const char* name)
{
	if (!name)
		return ElementLayout::Invalid;

	auto it = elementLayoutMap.find(name);
	if (it == elementLayoutMap.end())
		return ElementLayout::Invalid;

	return it->second;
}

ElementType elementTypeFromName(const char* name)
{
	if (!name)
		return ElementType::Invalid;

	auto it = elementTypeMap.find(name);
	if (it == elementTypeMap.end())
		return ElementType::Invalid;

	return it->second;
}

std::size_t elementLayoutSize(ElementLayout layout)
{
	auto index = static_cast<unsigned int>(layout);
	if (index >= elementLayoutCount)
		return 0;

	return elementLayoutSizes[index];
}

bool isElementValid(ElementLayout layout, ElementType type)
{
	switch (layout)
	{
		case ElementLayout::X8:
		case ElementLayout::X8Y8:
		case ElementLayout::X8Y8Z8:
		case ElementLayout::X8Y8Z8W8:
		case ElementLayout::W2X10Y10Z10:
		case ElementLayout::W2Z10Y10X10:
			switch (type)
			{
				case ElementType::UNorm:
				case ElementType::SNorm:
				case ElementType::UInt:
				case ElementType::SInt:
					return true;
				default:
					return false;
			}
		case ElementLayout::X16:
		case ElementLayout::X16Y16:
		case ElementLayout::X16Y16Z16:
		case ElementLayout::X16Y16Z16W16:
			return type != ElementType::Invalid;
		case ElementLayout::X32:
		case ElementLayout::X32Y32:
		case ElementLayout::X32Y32Z32:
		case ElementLayout::X32Y32Z32W32:
		case ElementLayout::X64:
		case ElementLayout::X64Y64:
		case ElementLayout::X64Y64Z64:
		case ElementLayout::X64Y64Z64W64:
			switch (type)
			{
				case ElementType::UInt:
				case ElementType::SInt:
				case ElementType::Float:
					return true;
				default:
					return false;
			}
		case ElementLayout::Z10Y11X11_UFloat:
		case ElementLayout::E5Z9Y9X9_UFloat:
			return type == ElementType::Float;
		default:
			return false;
	}
}

} // namespace vfc
