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

#include <VFC/Converter.h>
#include <gtest/gtest.h>
#include <utility>

#if VFC_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <glm/gtc/packing.hpp>
#include <glm/glm.hpp>

#if VFC_GCC
#pragma GCC diagnostic pop
#endif

TEST(ConverterTest, QuadWithIndices)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 6));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(1U, indices.size());
	ASSERT_EQ(6U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(4U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, QuadWithoutIndices)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::NoIndices,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 6));
	ASSERT_TRUE(converter.convert());

	EXPECT_TRUE(converter.getIndices().empty());

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(6U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, QuadRemapIndices)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f
	};

	std::uint32_t positionIndices[] = {0, 1, 2, 2, 1, 3};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};

	std::uint16_t texCoordIndices[] = {3, 2, 1, 1, 2, 0};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 4,
		vfc::IndexType::UInt32, positionIndices, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 4,
		vfc::IndexType::UInt16, texCoordIndices, 6));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(1U, indices.size());
	ASSERT_EQ(6U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(4U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, NormalizeToBoundsUNorm)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.25f, 0.125f,
		1.25f, 0.125f,
		0.25f, 1.125f,
		0.25f, 1.125f,
		1.25f, 0.125f,
		1.25f, 1.125f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 6));
	EXPECT_FALSE(converter.setElementTransform("asdf", vfc::Converter::Transform::Bounds));
	ASSERT_TRUE(converter.setElementTransform("texCoords", vfc::Converter::Transform::Bounds));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(1U, indices.size());
	ASSERT_EQ(6U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(4U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.25, 0.125), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.25, 1.125), maxBounds);
}

TEST(ConverterTest, NormalizeToBoundsSNorm)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.25f, 0.125f,
		1.25f, 0.125f,
		0.25f, 1.125f,
		0.25f, 1.125f,
		1.25f, 0.125f,
		1.25f, 1.125f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::SNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 6));
	EXPECT_FALSE(converter.setElementTransform("asdf", vfc::Converter::Transform::Bounds));
	ASSERT_TRUE(converter.setElementTransform("texCoords", vfc::Converter::Transform::Bounds));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(1U, indices.size());
	ASSERT_EQ(6U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(4U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0x8000, texCoord[0]);
	EXPECT_EQ(0x8000, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0x7FFF, texCoord[0]);
	EXPECT_EQ(0x8000, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0x8000, texCoord[0]);
	EXPECT_EQ(0x7FFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0x7FFF, texCoord[0]);
	EXPECT_EQ(0x7FFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.25, 0.125), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.25, 1.125), maxBounds);
}

TEST(ConverterTest, UNormToSNorm)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::SNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 6));
	EXPECT_FALSE(converter.setElementTransform("asdf", vfc::Converter::Transform::UNormToSNorm));
	ASSERT_TRUE(converter.setElementTransform("texCoords", vfc::Converter::Transform::UNormToSNorm));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(1U, indices.size());
	ASSERT_EQ(6U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(4U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0x8000, texCoord[0]);
	EXPECT_EQ(0x8000, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0x7FFF, texCoord[0]);
	EXPECT_EQ(0x8000, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0x8000, texCoord[0]);
	EXPECT_EQ(0x7FFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0x7FFF, texCoord[0]);
	EXPECT_EQ(0x7FFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, SNormToUNorm)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), positions, 6));
	EXPECT_FALSE(converter.setElementTransform("asdf", vfc::Converter::Transform::UNormToSNorm));
	ASSERT_TRUE(converter.setElementTransform("texCoords", vfc::Converter::Transform::UNormToSNorm));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(1U, indices.size());
	ASSERT_EQ(6U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(4U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, PointListWithMaxIndexValue)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,

		 1.0f, -1.0f,
		 2.0f, -1.0f,
		 1.0f,  1.0f,
		 1.0f,  1.0f,
		 2.0f, -1.0f,
		 2.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::PointList, 0, 6);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 12));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 12));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(2U, indices.size());
	ASSERT_EQ(9U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0, indices[0].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));
	EXPECT_EQ(4U, vfc::getIndexValue(indices[0].type, indices[0].data, 6));
	EXPECT_EQ(5U, vfc::getIndexValue(indices[0].type, indices[0].data, 7));
	EXPECT_EQ(6U, vfc::getIndexValue(indices[0].type, indices[0].data, 8));

	ASSERT_EQ(3U, indices[1].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[1].type);
	EXPECT_EQ(7, indices[1].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[1].type, indices[1].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[1].type, indices[1].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[1].type, indices[1].data, 2));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(10U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(2.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, LineListWithMaxIndexValue)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,

		 1.0f, -1.0f,
		 2.0f, -1.0f,
		 1.0f,  1.0f,
		 2.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	std::uint32_t inputIndices[] = {0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::LineList, 0, 5);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 8,
		vfc::IndexType::UInt32, inputIndices, 12));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 8,
		vfc::IndexType::UInt32, inputIndices, 12));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(2U, indices.size());
	ASSERT_EQ(8U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0, indices[0].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));
	EXPECT_EQ(4U, vfc::getIndexValue(indices[0].type, indices[0].data, 6));
	EXPECT_EQ(5U, vfc::getIndexValue(indices[0].type, indices[0].data, 7));

	ASSERT_EQ(4U, indices[1].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[1].type);
	EXPECT_EQ(6, indices[1].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[1].type, indices[1].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[1].type, indices[1].data, 1));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[1].type, indices[1].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[1].type, indices[1].data, 3));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(9U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(2.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, LineStripWithMaxIndexValue)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,

		 1.0f, -1.0f,
		 2.0f, -1.0f,
		 1.0f,  1.0f,
		 2.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	std::uint32_t inputIndices[] = {0, 1, 2, 3, 0xFFFFFFFF, 4, 5, 6, 7};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::LineStrip, 0, 5);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 8,
		vfc::IndexType::UInt32, inputIndices, 9));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 8,
		vfc::IndexType::UInt32, inputIndices, 9));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(2U, indices.size());
	ASSERT_EQ(7U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0, indices[0].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(0xFFFFU, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(4U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));
	EXPECT_EQ(5U, vfc::getIndexValue(indices[0].type, indices[0].data, 6));

	ASSERT_EQ(3U, indices[1].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[1].type);
	EXPECT_EQ(6, indices[1].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[1].type, indices[1].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[1].type, indices[1].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[1].type, indices[1].data, 2));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(9U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(2.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, TrinagleListWithMaxIndexValue)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,

		 1.0f, -1.0f,
		 2.0f, -1.0f,
		 1.0f,  1.0f,
		 2.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	std::uint32_t inputIndices[] = {0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList, 0, 6);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 8,
		vfc::IndexType::UInt32, inputIndices, 12));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 8,
		vfc::IndexType::UInt32, inputIndices, 12));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(2U, indices.size());
	ASSERT_EQ(9U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0, indices[0].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));
	EXPECT_EQ(4U, vfc::getIndexValue(indices[0].type, indices[0].data, 6));
	EXPECT_EQ(5U, vfc::getIndexValue(indices[0].type, indices[0].data, 7));
	EXPECT_EQ(6U, vfc::getIndexValue(indices[0].type, indices[0].data, 8));

	ASSERT_EQ(3U, indices[1].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[1].type);
	EXPECT_EQ(7, indices[1].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(10U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(2.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, TrinagleStripWithMaxIndexValue)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,

		 1.0f, -1.0f,
		 2.0f, -1.0f,
		 1.0f,  1.0f,
		 2.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	std::uint32_t inputIndices[] = {0, 1, 2, 3, 0xFFFFFFFF, 4, 5, 6, 7};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleStrip, 0, 6);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 8,
		vfc::IndexType::UInt32, inputIndices, 9));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 8,
		vfc::IndexType::UInt32, inputIndices, 9));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(2U, indices.size());
	ASSERT_EQ(8U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0, indices[0].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(0xFFFFU, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(4U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));
	EXPECT_EQ(5U, vfc::getIndexValue(indices[0].type, indices[0].data, 6));
	EXPECT_EQ(6U, vfc::getIndexValue(indices[0].type, indices[0].data, 7));

	ASSERT_EQ(3U, indices[1].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[1].type);
	EXPECT_EQ(7, indices[1].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(10U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(2.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, TrinagleFanWithMaxIndexValue)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f,

		 1.0f, -1.0f,
		 2.0f, -1.0f,
		 2.0f,  1.0f,
		 1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	std::uint32_t inputIndices[] = {0, 1, 2, 3, 0xFFFFFFFF, 4, 5, 6, 7};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleFan, 0, 6);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 8,
		vfc::IndexType::UInt32, inputIndices, 9));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 8,
		vfc::IndexType::UInt32, inputIndices, 9));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(2U, indices.size());
	ASSERT_EQ(8U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0, indices[0].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(0xFFFFU, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(4U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));
	EXPECT_EQ(5U, vfc::getIndexValue(indices[0].type, indices[0].data, 6));
	EXPECT_EQ(6U, vfc::getIndexValue(indices[0].type, indices[0].data, 7));

	ASSERT_EQ(3U, indices[1].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[1].type);
	EXPECT_EQ(7, indices[1].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(10U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(2.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, PatchListWithMaxIndexValue)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,

		 1.0f, -1.0f,
		 2.0f, -1.0f,
		 1.0f,  1.0f,
		 2.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::PatchList, 4, 5);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 8));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 8));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(2U, indices.size());
	ASSERT_EQ(4U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0, indices[0].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));

	ASSERT_EQ(4U, indices[1].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[1].type);
	EXPECT_EQ(4, indices[1].baseVertex);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));

	ASSERT_EQ(1U, converter.getVertices().size());
	const std::vector<std::uint8_t>& vertices = converter.getVertices()[0];
	ASSERT_EQ(8U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat.stride(), vertices.size());

	const std::uint8_t* vertexData = vertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	vertexData += vertexFormat.stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(vertexData + vertexFormat[0].offset));
	EXPECT_EQ(2.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoord = reinterpret_cast<const std::uint16_t*>(vertexData + vertexFormat[1].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(vertices.data() + vertices.size(), vertexData + vertexFormat.stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(2.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, ConstructorErrors)
{
	{
		std::vector<std::string> errors;
		vfc::Converter converter(vfc::VertexFormat(), vfc::IndexType::UInt16,
			vfc::PrimitiveType::Invalid, 0,
			[&errors](const char* message) {errors.push_back(message);});

		std::vector<std::string> expectedErrors =
		{
			"Converter vertex format is empty.",
			"Converter primitive type is invalid.",
		};
		EXPECT_FALSE(converter.isValid());
		EXPECT_EQ(expectedErrors, errors);
	}

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);
	{
		std::vector<std::string> errors;
		vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
			vfc::PrimitiveType::PatchList, 0,
			[&errors](const char* message) {errors.push_back(message);});

		std::vector<std::string> expectedErrors =
		{
			"Patch point count must be provided to Converter when using PatchList primitives.",
		};
		EXPECT_FALSE(converter.isValid());
		EXPECT_EQ(expectedErrors, errors);
	}

	{
		std::vector<std::pair<vfc::PrimitiveType, std::uint32_t>> primitiveIndexCounts =
		{
			{vfc::PrimitiveType::LineList, 2},
			{vfc::PrimitiveType::LineStrip, 2},
			{vfc::PrimitiveType::TriangleList, 3},
			{vfc::PrimitiveType::TriangleStrip, 3},
			{vfc::PrimitiveType::TriangleFan, 3},
			{vfc::PrimitiveType::PatchList, 4}
		};

		for (const auto& primitiveIndexCount : primitiveIndexCounts)
		{
			std::vector<std::string> errors;
			vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
				primitiveIndexCount.first, 4, primitiveIndexCount.second - 2,
				[&errors](const char* message) {errors.push_back(message);});

			std::vector<std::string> expectedErrors =
			{
				"Max index value is too small to hold any primitives."
			};
			EXPECT_FALSE(converter.isValid());
			EXPECT_EQ(expectedErrors, errors);

			errors.clear();
			converter = vfc::Converter(vertexFormat, vfc::IndexType::UInt16,
				primitiveIndexCount.first, 4, primitiveIndexCount.second - 1,
				[&errors](const char* message) {errors.push_back(message);});
			EXPECT_TRUE(converter.isValid());
			EXPECT_TRUE(errors.empty());
		}
	}

	{
		std::vector<std::string> errors;
		vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
			vfc::PrimitiveType::TriangleList, 0, 0xFFFFFFFF,
			[&errors](const char* message) {errors.push_back(message);});

		std::vector<std::string> expectedErrors =
		{
			"Max index value is higher than the maximum for the type.",
		};
		EXPECT_FALSE(converter.isValid());
		EXPECT_EQ(expectedErrors, errors);
	}
}

TEST(ConverterTest, BadVertexCount)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		1.0f, -1.0f,
		1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	std::vector<std::string> errors;
	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList, 0,
		[&errors](const char* message) {errors.push_back(message);});
	EXPECT_FALSE(converter.addVertexStream(std::move(positionFormat), positions, 5));
	EXPECT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 6));
	EXPECT_FALSE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 3));
	EXPECT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 6));

	std::vector<std::string> expectedErrors =
	{
		"Invalid non-indexed vertex count for requested primitive.",
		"Mismatch between number of non-indexed vertices for vertex streams."
	};
	EXPECT_EQ(expectedErrors, errors);
}

TEST(ConverterTest, BadIndexCount)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f
	};

	std::uint32_t positionIndices[] = {0, 1, 2, 2, 1, 3};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
	};

	std::uint32_t texCoordIndices[] = {3, 2, 1, 1, 2, 0};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	std::vector<std::string> errors;
	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList, 0,
		[&errors](const char* message) {errors.push_back(message);});
	EXPECT_FALSE(converter.addVertexStream(positionFormat, positions, 4, vfc::IndexType::UInt32,
		positionIndices, 5));
	EXPECT_TRUE(converter.addVertexStream(positionFormat, positions, 4, vfc::IndexType::UInt32,
		positionIndices, 6));
	EXPECT_FALSE(converter.addVertexStream(texCoordFormat, texCoords, 4, vfc::IndexType::UInt32,
		texCoordIndices, 3));
	EXPECT_TRUE(converter.addVertexStream(texCoordFormat, texCoords, 4, vfc::IndexType::UInt32,
		texCoordIndices, 6));

	std::vector<std::string> expectedErrors =
	{
		"Invalid index count for requested primitive.",
		"Mismatch between number of indices for vertex streams."
	};
	EXPECT_EQ(expectedErrors, errors);
}

TEST(ConverterTest, BadVertexStreams)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		1.0f, -1.0f,
		1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	std::vector<std::string> errors;
	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList, 0,
		[&errors](const char* message) {errors.push_back(message);});
	EXPECT_TRUE(converter.addVertexStream(positionFormat, positions, 6));
	EXPECT_FALSE(converter.addVertexStream(positionFormat, positions, 6));
	EXPECT_FALSE(converter.convert());

	std::vector<std::string> expectedErrors =
	{
		"Vertex element 'positions' is present in multiple vertex streams.",
		"Vertex element 'texCoords' has no corresponding input vertex stream."
	};
	EXPECT_EQ(expectedErrors, errors);
}

TEST(ConverterTest, OutOfRangeIndices)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f
	};

	std::uint32_t positionIndices[] = {0, 1, 2, 2, 1, 3};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
	};

	std::uint32_t texCoordIndices[] = {3, 2, 1, 1, 2, 0};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	std::vector<std::string> errors;
	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList, 0,
		[&errors](const char* message) {errors.push_back(message);});
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 3,
		vfc::IndexType::UInt32, positionIndices, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 4,
		vfc::IndexType::UInt32, texCoordIndices, 6));
	EXPECT_FALSE(converter.convert());

	std::vector<std::string> expectedErrors =
	{
		"Index value for vertex element 'positions' is out of range."
	};
	EXPECT_EQ(expectedErrors, errors);
}

TEST(ConverterTest, PrimitiveRestartWithoutIndices)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,

		 1.0f, -1.0f,
		 2.0f, -1.0f,
		 1.0f,  1.0f,
		 2.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
	};

	std::uint32_t inputIndices[] = {0, 1, 2, 3, 0xFFFFFFFF, 4, 5, 6, 7};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	vfc::VertexFormat vertexFormat;
	vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	std::vector<std::string> errors;
	vfc::Converter converter(vertexFormat, vfc::IndexType::NoIndices,
		vfc::PrimitiveType::LineStrip, 0,
		[&errors](const char* message) {errors.push_back(message);});
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 8,
		vfc::IndexType::UInt32, inputIndices, 9));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 8,
		vfc::IndexType::UInt32, inputIndices, 9));
	EXPECT_FALSE(converter.convert());

	std::vector<std::string> expectedErrors =
	{
		"Indices must be output if a primitive restart is used."
	};
	EXPECT_EQ(expectedErrors, errors);
}

TEST(ConverterTest, QuadWithIndicesSplitStreams)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	std::vector<vfc::VertexFormat> vertexFormat(2);
	vertexFormat[0].appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat[1].appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 6));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(1U, indices.size());
	ASSERT_EQ(6U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));

	ASSERT_EQ(2U, converter.getVertices().size());
	const std::vector<std::uint8_t>& positionVertices = converter.getVertices()[0];
	const std::vector<std::uint8_t>& texCoordVertices = converter.getVertices()[1];
	ASSERT_EQ(4U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat[0].stride(), positionVertices.size());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat[1].stride(), texCoordVertices.size());

	const std::uint8_t* positionVertexData = positionVertices.data();
	const std::uint8_t* texCoordVertexData = texCoordVertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(positionVertices.data() + positionVertices.size(),
		positionVertexData + vertexFormat[0].stride());
	EXPECT_EQ(texCoordVertices.data() + texCoordVertices.size(),
		texCoordVertexData + vertexFormat[0].stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, QuadWithoutIndicesSplitStreams)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	std::vector<vfc::VertexFormat> vertexFormat(2);
	vertexFormat[0].appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat[1].appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::NoIndices,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 6));
	ASSERT_TRUE(converter.convert());

	EXPECT_TRUE(converter.getIndices().empty());

	ASSERT_EQ(2U, converter.getVertices().size());
	const std::vector<std::uint8_t>& positionVertices = converter.getVertices()[0];
	const std::vector<std::uint8_t>& texCoordVertices = converter.getVertices()[1];
	ASSERT_EQ(6U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat[0].stride(), positionVertices.size());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat[1].stride(), texCoordVertices.size());

	const std::uint8_t* positionVertexData = positionVertices.data();
	const std::uint8_t* texCoordVertexData = texCoordVertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(positionVertices.data() + positionVertices.size(),
		positionVertexData + vertexFormat[0].stride());
	EXPECT_EQ(texCoordVertices.data() + texCoordVertices.size(),
		texCoordVertexData + vertexFormat[0].stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}

TEST(ConverterTest, QuadRemapIndicesSplitStreams)
{
	float positions[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f
	};

	std::uint32_t positionIndices[] = {0, 1, 2, 2, 1, 3};

	vfc::VertexFormat positionFormat;
	positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	float texCoords[] =
	{
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};

	std::uint16_t texCoordIndices[] = {3, 2, 1, 1, 2, 0};

	vfc::VertexFormat texCoordFormat;
	texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

	std::vector<vfc::VertexFormat> vertexFormat(2);
	vertexFormat[0].appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
	vertexFormat[1].appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

	vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16,
		vfc::PrimitiveType::TriangleList);
	ASSERT_TRUE(converter.addVertexStream(std::move(positionFormat), positions, 4,
		vfc::IndexType::UInt32, positionIndices, 6));
	ASSERT_TRUE(converter.addVertexStream(std::move(texCoordFormat), texCoords, 4,
		vfc::IndexType::UInt16, texCoordIndices, 6));
	ASSERT_TRUE(converter.convert());

	const std::vector<vfc::IndexData>& indices = converter.getIndices();
	ASSERT_EQ(1U, indices.size());
	ASSERT_EQ(6U, indices[0].count);
	EXPECT_EQ(vfc::IndexType::UInt16, indices[0].type);
	EXPECT_EQ(0U, vfc::getIndexValue(indices[0].type, indices[0].data, 0));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 1));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 2));
	EXPECT_EQ(2U, vfc::getIndexValue(indices[0].type, indices[0].data, 3));
	EXPECT_EQ(1U, vfc::getIndexValue(indices[0].type, indices[0].data, 4));
	EXPECT_EQ(3U, vfc::getIndexValue(indices[0].type, indices[0].data, 5));

	ASSERT_EQ(2U, converter.getVertices().size());
	const std::vector<std::uint8_t>& positionVertices = converter.getVertices()[0];
	const std::vector<std::uint8_t>& texCoordVertices = converter.getVertices()[1];
	ASSERT_EQ(4U, converter.getVertexCount());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat[0].stride(), positionVertices.size());
	ASSERT_EQ(converter.getVertexCount()*vertexFormat[1].stride(), texCoordVertices.size());

	const std::uint8_t* positionVertexData = positionVertices.data();
	const std::uint8_t* texCoordVertexData = texCoordVertices.data();
	glm::vec2 position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	auto texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(-1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0U, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(-1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0U, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	positionVertexData += vertexFormat[0].stride();
	position = glm::unpackHalf(
		*reinterpret_cast<const glm::u16vec2*>(positionVertexData + vertexFormat[0][0].offset));
	EXPECT_EQ(1.0f, position.x);
	EXPECT_EQ(1.0f, position.y);
	texCoordVertexData += vertexFormat[1].stride();
	texCoord =
		reinterpret_cast<const std::uint16_t*>(texCoordVertexData + vertexFormat[1][0].offset);
	EXPECT_EQ(0xFFFF, texCoord[0]);
	EXPECT_EQ(0xFFFF, texCoord[1]);

	EXPECT_EQ(positionVertices.data() + positionVertices.size(),
		positionVertexData + vertexFormat[0].stride());
	EXPECT_EQ(texCoordVertices.data() + texCoordVertices.size(),
		texCoordVertexData + vertexFormat[0].stride());

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(0.0, 0.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
}
