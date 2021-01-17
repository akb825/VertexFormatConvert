/*
 * Copyright 2020-2021 Aaron Barany
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
	std::vector<vfc::VertexFormat> vertexFormat(2);
	ASSERT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		vertexFormat[0].appendElement("position", vfc::ElementLayout::X16Y16Z16W16,
			vfc::ElementType::Float));
	ASSERT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		vertexFormat[1].appendElement("texCoord", vfc::ElementLayout::X16Y16,
			vfc::ElementType::UNorm));

	std::vector<std::vector<Bounds>> bounds =
	{
		{Bounds{vfc::VertexValue(-1, -2, -3, -4), vfc::VertexValue(1, 2, 3, 4)}},
		{Bounds{vfc::VertexValue(0, 0), vfc::VertexValue(1, 1)}}
	};

	std::vector<std::string> vertexData = {"positions.dat", "texCoords.dat"};

	std::string result = resultFile(vertexFormat, bounds, vertexData, 6, vfc::IndexType::NoIndices,
		{});

	const char* expectedResult =
		"{\n"
		"    \"vertices\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"position\",\n"
		"                    \"layout\": \"X16Y16Z16W16\",\n"
		"                    \"type\": \"Float\",\n"
		"                    \"offset\": 0,\n"
		"                    \"minValue\": [\n"
		"                        -1.0,\n"
		"                        -2.0,\n"
		"                        -3.0,\n"
		"                        -4.0\n"
		"                    ],\n"
		"                    \"maxValue\": [\n"
		"                        1.0,\n"
		"                        2.0,\n"
		"                        3.0,\n"
		"                        4.0\n"
		"                    ]\n"
		"                }\n"
		"            ],\n"
		"            \"vertexStride\": 8,\n"
		"            \"vertexData\": \"positions.dat\"\n"
		"        },\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"texCoord\",\n"
		"                    \"layout\": \"X16Y16\",\n"
		"                    \"type\": \"UNorm\",\n"
		"                    \"offset\": 0,\n"
		"                    \"minValue\": [\n"
		"                        0.0,\n"
		"                        0.0,\n"
		"                        0.0,\n"
		"                        1.0\n"
		"                    ],\n"
		"                    \"maxValue\": [\n"
		"                        1.0,\n"
		"                        1.0,\n"
		"                        0.0,\n"
		"                        1.0\n"
		"                    ]\n"
		"                }\n"
		"            ],\n"
		"            \"vertexStride\": 4,\n"
		"            \"vertexData\": \"texCoords.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexCount\": 6\n"
		"}";
	EXPECT_EQ(expectedResult, result);
}

TEST(ResultFileTest, WithIndices)
{
	std::vector<vfc::VertexFormat> vertexFormat(2);
	ASSERT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		vertexFormat[0].appendElement("position", vfc::ElementLayout::X16Y16Z16W16,
			vfc::ElementType::Float));
	ASSERT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		vertexFormat[1].appendElement("texCoord", vfc::ElementLayout::X16Y16,
			vfc::ElementType::UNorm));

	std::vector<std::vector<Bounds>> bounds =
	{
		{Bounds{vfc::VertexValue(-1, -2, -3, -4), vfc::VertexValue(1, 2, 3, 4)}},
		{Bounds{vfc::VertexValue(0, 0), vfc::VertexValue(1, 1)}}
	};

	std::vector<std::string> vertexData = {"positions.dat", "texCoords.dat"};

	std::vector<IndexFileData> indexData =
	{
		IndexFileData{6, 0, "indices.0.dat"},
		IndexFileData{6, 4, "indices.1.dat"}
	};

	std::string result = resultFile(vertexFormat, bounds, vertexData, 8, vfc::IndexType::UInt16,
		indexData);

	const char* expectedResult =
		"{\n"
		"    \"vertices\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"position\",\n"
		"                    \"layout\": \"X16Y16Z16W16\",\n"
		"                    \"type\": \"Float\",\n"
		"                    \"offset\": 0,\n"
		"                    \"minValue\": [\n"
		"                        -1.0,\n"
		"                        -2.0,\n"
		"                        -3.0,\n"
		"                        -4.0\n"
		"                    ],\n"
		"                    \"maxValue\": [\n"
		"                        1.0,\n"
		"                        2.0,\n"
		"                        3.0,\n"
		"                        4.0\n"
		"                    ]\n"
		"                }\n"
		"            ],\n"
		"            \"vertexStride\": 8,\n"
		"            \"vertexData\": \"positions.dat\"\n"
		"        },\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"texCoord\",\n"
		"                    \"layout\": \"X16Y16\",\n"
		"                    \"type\": \"UNorm\",\n"
		"                    \"offset\": 0,\n"
		"                    \"minValue\": [\n"
		"                        0.0,\n"
		"                        0.0,\n"
		"                        0.0,\n"
		"                        1.0\n"
		"                    ],\n"
		"                    \"maxValue\": [\n"
		"                        1.0,\n"
		"                        1.0,\n"
		"                        0.0,\n"
		"                        1.0\n"
		"                    ]\n"
		"                }\n"
		"            ],\n"
		"            \"vertexStride\": 4,\n"
		"            \"vertexData\": \"texCoords.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexCount\": 8,\n"
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
