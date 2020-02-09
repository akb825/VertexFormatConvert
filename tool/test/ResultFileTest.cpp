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

#include "ResultFile.h"
#include <gtest/gtest.h>

TEST(ResultFileTest, NoIndices)
{
	vfc::VertexFormat vertexFormat;
	ASSERT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		vertexFormat.appendElement("position", vfc::ElementLayout::X16Y16Z16W16,
			vfc::ElementType::Float));
	ASSERT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		vertexFormat.appendElement("texCoord", vfc::ElementLayout::X16Y16,
			vfc::ElementType::UNorm));
	std::string result = resultFile(vertexFormat, 6, "vertices.dat", vfc::IndexType::NoIndices,
		nullptr, 0);

	const char* expectedResult =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"position\",\n"
		"            \"layout\": \"X16Y16Z16W16\",\n"
		"            \"type\": \"Float\",\n"
		"            \"offset\": 0\n"
		"        },\n"
		"        {\n"
		"            \"name\": \"texCoord\",\n"
		"            \"layout\": \"X16Y16\",\n"
		"            \"type\": \"UNorm\",\n"
		"            \"offset\": 8\n"
		"        }\n"
		"    ],\n"
		"    \"vertexStride\": 12,\n"
		"    \"vertexCount\": 6,\n"
		"    \"vertexData\": \"vertices.dat\"\n"
		"}";
	EXPECT_EQ(expectedResult, result);
}

TEST(ResultFileTest, WithIndices)
{
	vfc::VertexFormat vertexFormat;
	ASSERT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		vertexFormat.appendElement("position", vfc::ElementLayout::X16Y16Z16W16,
			vfc::ElementType::Float));
	ASSERT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		vertexFormat.appendElement("texCoord", vfc::ElementLayout::X16Y16,
			vfc::ElementType::UNorm));
	std::vector<IndexFileData> indexData =
	{
		IndexFileData{6, 0, "indices.0.dat"},
		IndexFileData{6, 4, "indices.1.dat"}
	};
	std::string result = resultFile(vertexFormat, 8, "vertices.dat", vfc::IndexType::UInt16,
		indexData.data(), indexData.size());

	const char* expectedResult =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"position\",\n"
		"            \"layout\": \"X16Y16Z16W16\",\n"
		"            \"type\": \"Float\",\n"
		"            \"offset\": 0\n"
		"        },\n"
		"        {\n"
		"            \"name\": \"texCoord\",\n"
		"            \"layout\": \"X16Y16\",\n"
		"            \"type\": \"UNorm\",\n"
		"            \"offset\": 8\n"
		"        }\n"
		"    ],\n"
		"    \"vertexStride\": 12,\n"
		"    \"vertexCount\": 8,\n"
		"    \"vertexData\": \"vertices.dat\",\n"
		"    \"indexType\": \"UInt16\",\n"
		"    \"indexBuffers\": [\n"
		"        {\n"
		"            \"indexCount\": 6,\n"
		"            \"baseVertex\": 0,\n"
		"            \"indexData\": \"indices.0.dat\"\n"
		"        },\n"
		"        {\n"
		"            \"indexCount\": 6,\n"
		"            \"baseVertex\": 4,\n"
		"            \"indexData\": \"indices.1.dat\"\n"
		"        }\n"
		"    ]\n"
		"}";
	EXPECT_EQ(expectedResult, result);
}
