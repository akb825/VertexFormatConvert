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

#include <VFC/Converter.h>
#include <gtest/gtest.h>

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

	const std::vector<std::uint8_t>& vertices = converter.getVertices();
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

	const std::vector<std::uint8_t>& vertices = converter.getVertices();
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
		0.0f, 0.0f,
	};

	std::uint32_t texCoordIndices[] = {3, 2, 1, 1, 2, 0};

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
		vfc::IndexType::UInt32, texCoordIndices, 6));
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

	const std::vector<std::uint8_t>& vertices = converter.getVertices();
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

	const std::vector<std::uint8_t>& vertices = converter.getVertices();
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

	const std::vector<std::uint8_t>& vertices = converter.getVertices();
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

	const std::vector<std::uint8_t>& vertices = converter.getVertices();
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

	const std::vector<std::uint8_t>& vertices = converter.getVertices();
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

	vfc::VertexValue minBounds, maxBounds;
	EXPECT_FALSE(converter.getVertexElementBounds(minBounds, maxBounds, "asdf"));
	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "positions"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);

	EXPECT_TRUE(converter.getVertexElementBounds(minBounds, maxBounds, "texCoords"));
	EXPECT_EQ(vfc::VertexValue(-1.0, -1.0), minBounds);
	EXPECT_EQ(vfc::VertexValue(1.0, 1.0), maxBounds);
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
