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
#include <ctype.h>
#include <gtest/gtest.h>

TEST(VertexFormatTest, ElementLayoutMapping)
{
	EXPECT_EQ(nullptr, vfc::elementLayoutName(vfc::ElementLayout::Invalid));
	EXPECT_EQ(vfc::ElementLayout::Invalid, vfc::elementLayoutFromName("asdf"));
	EXPECT_EQ(vfc::ElementLayout::Invalid, vfc::elementLayoutFromName(nullptr));

	for (unsigned int i = 0; i < vfc::elementLayoutCount; ++i)
	{
		auto elementLayout = static_cast<vfc::ElementLayout>(i);
		EXPECT_STRNE(vfc::elementLayoutName(elementLayout),
			vfc::elementLayoutName(elementLayout, true));
		EXPECT_EQ(elementLayout, vfc::elementLayoutFromName(vfc::elementLayoutName(elementLayout)));
		EXPECT_EQ(elementLayout,
			vfc::elementLayoutFromName(vfc::elementLayoutName(elementLayout, true)));

		std::string lowercaseName = vfc::elementLayoutName(elementLayout);
		for (char& c : lowercaseName)
			c = static_cast<char>(tolower(c));
		EXPECT_EQ(elementLayout, vfc::elementLayoutFromName(lowercaseName.c_str()));

		lowercaseName = vfc::elementLayoutName(elementLayout, true);
		for (char& c : lowercaseName)
			c = static_cast<char>(tolower(c));
		EXPECT_EQ(elementLayout, vfc::elementLayoutFromName(lowercaseName.c_str()));
	}
}

TEST(VertexFormatTest, ElementTypeMapping)
{
	EXPECT_EQ(nullptr, vfc::elementTypeName(vfc::ElementType::Invalid));
	EXPECT_EQ(vfc::ElementType::Invalid, vfc::elementTypeFromName("asdf"));
	EXPECT_EQ(vfc::ElementType::Invalid, vfc::elementTypeFromName(nullptr));

	for (unsigned int i = 0; i < vfc::elementTypeCount; ++i)
	{
		auto elementType = static_cast<vfc::ElementType>(i);
		EXPECT_EQ(elementType, vfc::elementTypeFromName(vfc::elementTypeName(elementType)));

		std::string lowercaseName = vfc::elementTypeName(elementType);
		for (char& c : lowercaseName)
			c = static_cast<char>(tolower(c));
		EXPECT_EQ(elementType, vfc::elementTypeFromName(lowercaseName.c_str()));
	}
}

TEST(VertexFormatTest, IsElementValid)
{
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Invalid, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Invalid, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Invalid, vfc::ElementType::SNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Invalid, vfc::ElementType::UInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Invalid, vfc::ElementType::SInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Invalid, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X8, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8, vfc::ElementType::SInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X8, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X8Y8, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8, vfc::ElementType::SInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X8Y8, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8, vfc::ElementType::SInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8W8, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8W8, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8W8, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8W8, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8W8, vfc::ElementType::SInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X8Y8Z8W8, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::W2X10Y10Z10, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::W2X10Y10Z10, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::W2X10Y10Z10, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::W2X10Y10Z10, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::W2X10Y10Z10, vfc::ElementType::SInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::W2X10Y10Z10, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::W2Z10Y10X10, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::W2Z10Y10X10, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::W2Z10Y10X10, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::W2Z10Y10X10, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::W2Z10Y10X10, vfc::ElementType::SInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::W2Z10Y10X10, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X16, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X16Y16, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16W16, vfc::ElementType::Invalid));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16W16, vfc::ElementType::UNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16W16, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16W16, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16W16, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X16Y16Z16W16, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32Y32, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32Y32, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32Y32, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32Y32, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32Y32, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32Y32, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32W32, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32W32, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32W32, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32W32, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32W32, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X32Y32Z32W32, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64Y64, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64Y64, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64Y64, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64Y64, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64Y64, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64Y64, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64W64, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64W64, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64W64, vfc::ElementType::SNorm));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64W64, vfc::ElementType::UInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64W64, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::X64Y64Z64W64, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Z10Y11X11_UFloat, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Z10Y11X11_UFloat, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Z10Y11X11_UFloat, vfc::ElementType::SNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Z10Y11X11_UFloat, vfc::ElementType::UInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::Z10Y11X11_UFloat, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::Z10Y11X11_UFloat, vfc::ElementType::Float));

	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::E5Z9Y9X9_UFloat, vfc::ElementType::Invalid));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::E5Z9Y9X9_UFloat, vfc::ElementType::UNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::E5Z9Y9X9_UFloat, vfc::ElementType::SNorm));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::E5Z9Y9X9_UFloat, vfc::ElementType::UInt));
	EXPECT_FALSE(vfc::isElementValid(vfc::ElementLayout::E5Z9Y9X9_UFloat, vfc::ElementType::SInt));
	EXPECT_TRUE(vfc::isElementValid(vfc::ElementLayout::E5Z9Y9X9_UFloat, vfc::ElementType::Float));
}

TEST(VertexFormatTest, PrimitiveTypeMapping)
{
	EXPECT_EQ(nullptr, vfc::primitiveTypeName(vfc::PrimitiveType::Invalid));
	EXPECT_EQ(vfc::PrimitiveType::Invalid, vfc::primitiveTypeFromName("asdf"));
	EXPECT_EQ(vfc::PrimitiveType::Invalid, vfc::primitiveTypeFromName(nullptr));

	for (unsigned int i = 0; i < vfc::primitiveTypeCount; ++i)
	{
		auto primitiveType = static_cast<vfc::PrimitiveType>(i);
		EXPECT_EQ(primitiveType, vfc::primitiveTypeFromName(vfc::primitiveTypeName(primitiveType)));

		std::string lowercaseName = vfc::primitiveTypeName(primitiveType);
		for (char& c : lowercaseName)
			c = static_cast<char>(tolower(c));
		EXPECT_EQ(primitiveType, vfc::primitiveTypeFromName(lowercaseName.c_str()));
	}
}

TEST(VertexFormatTest, IsVertexCountValid)
{
	EXPECT_TRUE(vfc::isVertexCountValid(vfc::PrimitiveType::PointList, 5));

	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::LineList, 5));
	EXPECT_TRUE(vfc::isVertexCountValid(vfc::PrimitiveType::LineList, 6));

	EXPECT_TRUE(vfc::isVertexCountValid(vfc::PrimitiveType::LineStrip, 5));
	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::LineStrip, 1));

	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleList, 4));
	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleList, 5));
	EXPECT_TRUE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleList, 6));

	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleStrip, 1));
	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleStrip, 2));
	EXPECT_TRUE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleStrip, 3));
	EXPECT_TRUE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleStrip, 6));

	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleFan, 1));
	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleFan, 2));
	EXPECT_TRUE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleFan, 3));
	EXPECT_TRUE(vfc::isVertexCountValid(vfc::PrimitiveType::TriangleFan, 6));

	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::PatchList, 1, 4));
	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::PatchList, 2, 4));
	EXPECT_FALSE(vfc::isVertexCountValid(vfc::PrimitiveType::PatchList, 3, 4));
	EXPECT_TRUE(vfc::isVertexCountValid(vfc::PrimitiveType::PatchList, 4, 4));
}

TEST(VertexFormatTest, AddElement)
{
	vfc::VertexFormat vertexFormat;
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded, vertexFormat.appendElement("foo",
		vfc::ElementLayout::R8G8B8A8, vfc::ElementType::UNorm));
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded, vertexFormat.appendElement("bar",
		vfc::ElementLayout::X32Y32Z32, vfc::ElementType::Float));
	EXPECT_EQ(vfc::VertexFormat::AddResult::NameNotUnique, vertexFormat.appendElement("bar",
		vfc::ElementLayout::X32Y32Z32, vfc::ElementType::Float));
	EXPECT_EQ(vfc::VertexFormat::AddResult::ElementInvalid, vertexFormat.appendElement("baz",
		vfc::ElementLayout::X32Y32Z32, vfc::ElementType::SNorm));

	EXPECT_EQ(sizeof(std::uint8_t)*4 + sizeof(float)*3, vertexFormat.stride());
	ASSERT_EQ(2U, vertexFormat.size());

	EXPECT_EQ("foo", vertexFormat[0].name);
	EXPECT_EQ(vfc::ElementLayout::R8G8B8A8, vertexFormat[0].layout);
	EXPECT_EQ(vfc::ElementType::UNorm, vertexFormat[0].type);
	EXPECT_EQ(0U, vertexFormat[0].offset);

	EXPECT_EQ("bar", vertexFormat[1].name);
	EXPECT_EQ(vfc::ElementLayout::X32Y32Z32, vertexFormat[1].layout);
	EXPECT_EQ(vfc::ElementType::Float, vertexFormat[1].type);
	EXPECT_EQ(sizeof(std::uint8_t)*4, vertexFormat[1].offset);

	EXPECT_EQ(vertexFormat.begin(), vertexFormat.find("foo"));
	EXPECT_EQ(vertexFormat.begin() + 1, vertexFormat.find("bar"));
	EXPECT_EQ(vertexFormat.end(), vertexFormat.find("baz"));
}

TEST(VertexFormatTest, ContainsElements)
{
	vfc::VertexFormat vertexFormat;
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded, vertexFormat.appendElement("foo",
		vfc::ElementLayout::R8G8B8A8, vfc::ElementType::UNorm));
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded, vertexFormat.appendElement("bar",
		vfc::ElementLayout::X32Y32Z32, vfc::ElementType::Float));
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded, vertexFormat.appendElement("baz",
		vfc::ElementLayout::W2X10Y10Z10, vfc::ElementType::UNorm));

	vfc::VertexFormat otherVertexFormat;
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded, otherVertexFormat.appendElement("foo",
		vfc::ElementLayout::X16Y16, vfc::ElementType::Float));
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded, otherVertexFormat.appendElement("bar",
		vfc::ElementLayout::Z10Y11X11_UFloat, vfc::ElementType::Float));

	EXPECT_TRUE(vertexFormat.containsElements(otherVertexFormat));
	EXPECT_FALSE(otherVertexFormat.containsElements(vertexFormat));
}
