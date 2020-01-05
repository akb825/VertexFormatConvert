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
