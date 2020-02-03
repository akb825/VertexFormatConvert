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


#include "ConfigFile.h"
#include <gtest/gtest.h>

TEST(ConfigFileTest, CompleteConfig)
{
	const char* json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"position\",\n"
		"            \"layout\": \"X32Y32\",\n"
		"            \"type\": \"float\"\n"
		"        },\n"
		"        {\n"
		"            \"name\": \"texCoord\",\n"
		"            \"layout\": \"X16Y16\",\n"
		"            \"type\": \"snorm\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"position\",\n"
		"                    \"layout\": \"x32y32z32\",\n"
		"                    \"type\": \"Float\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"positions.dat\",\n"
		"            \"indexType\": \"UInt32\",\n"
		"            \"indexData\": \"positionIndices.dat\"\n"
		"        },\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"texCoord\",\n"
		"                    \"layout\": \"x32y32\",\n"
		"                    \"type\": \"Float\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"texCoords.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexTransforms\": [\n"
		"        {\n"
		"            \"name\": \"texCoords\",\n"
		"            \"transform\": \"UNormToSNorm\"\n"
		"        }\n"
		"    ]\n"
		"}";

	ConfigFile configFile;
	ASSERT_TRUE(configFile.load(json, "foo.json"));

	vfc::VertexFormat expectedVertexFormat;
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		expectedVertexFormat.appendElement("position", vfc::ElementLayout::X32Y32,
			vfc::ElementType::Float));
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		expectedVertexFormat.appendElement("texCoord", vfc::ElementLayout::X16Y16,
			vfc::ElementType::SNorm));
	EXPECT_EQ(expectedVertexFormat, configFile.getVertexFormat());
	EXPECT_EQ(vfc::IndexType::UInt16, configFile.getIndexType());

	vfc::VertexFormat expectedFirstFormat;
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		expectedFirstFormat.appendElement("position", vfc::ElementLayout::X32Y32Z32,
			vfc::ElementType::Float));
	vfc::VertexFormat expectedSecondFormat;
	EXPECT_EQ(vfc::VertexFormat::AddResult::Succeeded,
		expectedSecondFormat.appendElement("texCoord", vfc::ElementLayout::X32Y32,
			vfc::ElementType::Float));
	std::vector<ConfigFile::VertexStream> expectedStreams =
	{
		ConfigFile::VertexStream{expectedFirstFormat, vfc::IndexType::UInt32, "positions.dat",
			"positionIndices.dat"},
		ConfigFile::VertexStream{expectedSecondFormat, vfc::IndexType::NoIndices, "texCoords.dat",
			""}
	};
	EXPECT_EQ(expectedStreams, configFile.getVertexStreams());

	std::vector<std::pair<std::string, vfc::Converter::Transform>> expectedTransforms =
		{{"texCoords", vfc::Converter::Transform::UNormToSNorm}};
	EXPECT_EQ(expectedTransforms, configFile.getTransforms());
}

TEST(ConfigFileTest, InvalidJson)
{
	const char* json =
		"{\n"
		"    \"test\": \"foo\"";

	std::vector<std::string> messages;
	ConfigFile configFile;
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	std::vector<std::string> expectedMessages =
	{
#if VFC_WINDOWS
		"foo.json(2, 18) : error: missing ',' or '}' after object member."
#else
		"foo.json:2:18: error: missing ',' or '}' after object member."
#endif
	};
	EXPECT_EQ(expectedMessages, messages);
}
