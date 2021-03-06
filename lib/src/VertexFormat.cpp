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

#include <algorithm>
#include <cctype>
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

const std::uint32_t elementLayoutSizes[] =
{
	static_cast<std::uint32_t>(sizeof(std::uint8_t)),    // X8
	static_cast<std::uint32_t>(sizeof(std::uint8_t)*2),  // X8Y8
	static_cast<std::uint32_t>(sizeof(std::uint8_t)*3),  // X8Y8Z8
	static_cast<std::uint32_t>(sizeof(std::uint8_t)*4),  // X8Y8Z8W8
	static_cast<std::uint32_t>(sizeof(std::uint32_t)),   // W2X10Y10Z1,
	static_cast<std::uint32_t>(sizeof(std::uint32_t)),   // W2Z10Y10X10
	static_cast<std::uint32_t>(sizeof(std::uint16_t)),   // X16
	static_cast<std::uint32_t>(sizeof(std::uint16_t)*2), // X16Y16
	static_cast<std::uint32_t>(sizeof(std::uint16_t)*3), // X16Y16Z16
	static_cast<std::uint32_t>(sizeof(std::uint16_t)*4), // X16Y16Z16W16
	static_cast<std::uint32_t>(sizeof(std::uint32_t)),   // X32
	static_cast<std::uint32_t>(sizeof(std::uint32_t)*2), // X32Y32
	static_cast<std::uint32_t>(sizeof(std::uint32_t)*3), // X32Y32Z32
	static_cast<std::uint32_t>(sizeof(std::uint32_t)*4), // X32Y32Z32W32
	static_cast<std::uint32_t>(sizeof(std::uint64_t)),   // X64
	static_cast<std::uint32_t>(sizeof(std::uint64_t)*2), // X64Y64
	static_cast<std::uint32_t>(sizeof(std::uint64_t)*3), // X64Y64Z64
	static_cast<std::uint32_t>(sizeof(std::uint64_t)*4), // X64Y64Z64W64
	static_cast<std::uint32_t>(sizeof(std::uint32_t)),   // Z10Y11X11_UFloat
	static_cast<std::uint32_t>(sizeof(std::uint32_t))    // E5Z9Y9X9_UFloat
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

const char* const primitiveTypeNames[] =
{
	"PointList",
	"LineList",
	"LineStrip",
	"TriangleList",
	"TriangleStrip",
	"TriangleFan",
	"PatchList"
};

static_assert(sizeof(primitiveTypeNames)/sizeof(*primitiveTypeNames) == primitiveTypeCount,
	"Unexpected primitiveTypeNames size.");

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

const std::unordered_map<StringRef, PrimitiveType, StringRefHash> primitiveTypeMap =
{
	{"PointList", PrimitiveType::PointList},
	{"LineList", PrimitiveType::LineList},
	{"LineStrip", PrimitiveType::LineStrip},
	{"TriangleList", PrimitiveType::TriangleList},
	{"TriangleStrip", PrimitiveType::TriangleStrip},
	{"TriangleFan", PrimitiveType::TriangleFan},
	{"PatchList", PrimitiveType::PatchList}
};

} // namespace

const char* elementLayoutName(ElementLayout layout, bool color)
{
	auto index = static_cast<unsigned int>(layout);
	if (index >= elementLayoutCount)
		return nullptr;

	return color ? colorElementLayoutNames[index] : elementLayoutNames[index];
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

const char* elementTypeName(ElementType type)
{
	auto index = static_cast<unsigned int>(type);
	if (index >= elementTypeCount)
		return nullptr;

	return elementTypeNames[index];
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

std::uint32_t elementLayoutSize(ElementLayout layout)
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

const char* primitiveTypeName(PrimitiveType type)
{
	auto index = static_cast<unsigned int>(type);
	if (index >= primitiveTypeCount)
		return nullptr;

	return primitiveTypeNames[index];
}

PrimitiveType primitiveTypeFromName(const char* name)
{
	if (!name)
		return PrimitiveType::Invalid;

	auto it = primitiveTypeMap.find(name);
	if (it == primitiveTypeMap.end())
		return PrimitiveType::Invalid;

	return it->second;
}

bool isVertexCountValid(PrimitiveType primitiveType, std::uint32_t vertexCount,
	unsigned int patchSize)
{
	switch (primitiveType)
	{
		case PrimitiveType::PointList:
			return true;
		case PrimitiveType::LineList:
			return (vertexCount & 1) == 0;
		case PrimitiveType::LineStrip:
			return vertexCount != 1;
		case PrimitiveType::TriangleList:
			return vertexCount % 3 == 0;
		case PrimitiveType::TriangleStrip:
		case PrimitiveType::TriangleFan:
			return vertexCount == 0 || vertexCount >= 3;
		case PrimitiveType::PatchList:
			assert(patchSize > 0);
			return vertexCount % patchSize == 0;
		default:
			return false;
	}
}

VertexFormat::AddResult VertexFormat::appendElement(std::string name, ElementLayout layout,
	ElementType type)
{
	if (!isElementValid(layout, type))
		return AddResult::ElementInvalid;

	if (find(name) != end())
		return AddResult::NameNotUnique;

	m_elements.push_back(VertexElement{std::move(name), layout, type, m_stride});
	m_stride += elementLayoutSize(layout);
	return AddResult::Succeeded;
}

VertexFormat::const_iterator VertexFormat::find(const char* name) const
{
	return std::find_if(m_elements.begin(), m_elements.end(),
		[name](const VertexElement& element) {return element.name == name;});
}

bool VertexFormat::containsElements(const VertexFormat& other) const
{
	for (const VertexElement& element : other)
	{
		if (find(element.name) == end())
			return false;
	}

	return true;
}

} // namespace vfc
