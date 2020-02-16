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
		"    \"primitiveType\": \"PatchList\",\n"
		"    \"patchPoints\": 3,\n"
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
	EXPECT_EQ(vfc::PrimitiveType::PatchList, configFile.getPrimitiveType());
	EXPECT_EQ(3U, configFile.getPatchPoints());

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
		"foo.json(2, 18) : error: Missing ',' or '}' after object member."
#else
		"foo.json:2:18: error: Missing ',' or '}' after object member."
#endif
	};
	EXPECT_EQ(expectedMessages, messages);
}

TEST(ConfigFileTest, InvalidVertexFormat)
{
	const char* json = "{}";

	std::vector<std::string> messages;
	ConfigFile configFile;
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	std::vector<std::string> expectedMessages =
	{
		"foo.json: error: Root must contain 'vertexFormat' member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": {}"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format must be an array."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [2]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format element must be an object."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [{}]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format element must contain 'name' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": 1\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format element must contain 'name' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\"\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format element must contain 'layout' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"bar\"\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format element layout 'bar' is invalid."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\"\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format element must contain 'type' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"bar\"\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format element type 'bar' is invalid."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"float\"\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format element layout 'r8g8b8a8' can't be used with type 'float'."
	};
	EXPECT_EQ(expectedMessages, messages);
}

TEST(ConfigFileTest, InvalidIndexType)
{
	const char* json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": 1\n"
		"}";

	std::vector<std::string> messages;
	ConfigFile configFile;
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	std::vector<std::string> expectedMessages =
	{
		"foo.json: error: Index type must be a string."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"foo\"\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Index type 'foo' is invalid."
	};
	EXPECT_EQ(expectedMessages, messages);
}

TEST(ConfigFileTest, InvalidPrimitiveType)
{
	const char* json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"primitiveType\": 1\n"
		"}";

	std::vector<std::string> messages;
	ConfigFile configFile;
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	std::vector<std::string> expectedMessages =
	{
		"foo.json: error: Primitive type must be a string."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"primitiveType\": \"foo\"\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Primitive type 'foo' is invalid."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"primitiveType\": \"PatchList\"\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Root must contain 'patchPoints' int member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"primitiveType\": \"PatchList\",\n"
		"    \"patchPoints\": \"foo\"\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Root must contain 'patchPoints' int member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"primitiveType\": \"PatchList\",\n"
		"    \"patchPoints\": 0\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Patch points must have a value > 0."
	};
	EXPECT_EQ(expectedMessages, messages);
}

TEST(ConfigFileTest, InvalidVertexStream)
{
	const char* json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\"\n"
		"}";

	std::vector<std::string> messages;
	ConfigFile configFile;
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	std::vector<std::string> expectedMessages =
	{
		"foo.json: error: Root must contain 'vertexStreams' member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": {}\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex streams must be an array."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [2]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex stream element must be an object."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [{}]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex stream element must contain 'vertexFormat' member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": 1\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex format must be an array."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ]\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex stream element must contain 'vertexData' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": 1\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex stream element must contain 'vertexData' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint8\"\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Index type 'uint8' is invalid."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint16\"\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex stream element must contain 'indexData' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint16\",\n"
		"            \"indexData\": 1\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex stream element must contain 'indexData' string member."
	};
	EXPECT_EQ(expectedMessages, messages);
}

TEST(ConfigFileTest, InvalidVertexTransform)
{
	const char* json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint16\",\n"
		"            \"indexData\": \"indices.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexTransforms\": {}"
		"}";

	std::vector<std::string> messages;
	ConfigFile configFile;
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	std::vector<std::string> expectedMessages =
	{
		"foo.json: error: Vertex transforms must be an array."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint16\",\n"
		"            \"indexData\": \"indices.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexTransforms\": [2]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex transform element must be an object."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint16\",\n"
		"            \"indexData\": \"indices.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexTransforms\": [{}]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex transform element must contain 'name' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint16\",\n"
		"            \"indexData\": \"indices.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexTransforms\": [\n"
		"        {\n"
		"            \"name\": 1\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex transform element must contain 'name' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint16\",\n"
		"            \"indexData\": \"indices.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexTransforms\": [\n"
		"        {\n"
		"            \"name\": \"foo\"\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex transform element must contain 'transform' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint16\",\n"
		"            \"indexData\": \"indices.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexTransforms\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"transform\": 1\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex transform element must contain 'transform' string member."
	};
	EXPECT_EQ(expectedMessages, messages);

	json =
		"{\n"
		"    \"vertexFormat\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"layout\": \"r8g8b8a8\",\n"
		"            \"type\": \"uint\"\n"
		"        }\n"
		"    ],\n"
		"    \"indexType\": \"uint16\",\n"
		"    \"vertexStreams\": [\n"
		"        {\n"
		"            \"vertexFormat\": [\n"
		"                {\n"
		"                    \"name\": \"foo\",\n"
		"                    \"layout\": \"r8g8b8a8\",\n"
		"                    \"type\": \"unorm\"\n"
		"                }\n"
		"            ],\n"
		"            \"vertexData\": \"vertices.dat\",\n"
		"            \"indexType\": \"uint16\",\n"
		"            \"indexData\": \"indices.dat\"\n"
		"        }\n"
		"    ],\n"
		"    \"vertexTransforms\": [\n"
		"        {\n"
		"            \"name\": \"foo\",\n"
		"            \"transform\": \"bar\"\n"
		"        }\n"
		"    ]\n"
		"}";

	messages.clear();
	EXPECT_FALSE(configFile.load(json, "foo.json",
		[&messages](const char* message) {messages.push_back(message);}));

	expectedMessages =
	{
		"foo.json: error: Vertex transform 'bar' is invalid."
	};
	EXPECT_EQ(expectedMessages, messages);
}
