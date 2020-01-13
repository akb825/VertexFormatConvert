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
