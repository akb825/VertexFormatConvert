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
#include <VFC/VertexValue.h>
#include <gtest/gtest.h>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

#if VFC_MSC
// Truncation of constant value.
#pragma warning(disable: 4309)
#endif

#if VFC_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <glm/gtc/packing.hpp>
#include <glm/glm.hpp>

#if VFC_GCC
#pragma GCC diagnostic pop
#endif

#if VFC_CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

static std::ostream& operator<<(std::ostream& stream, const vfc::VertexValue& value)
{
	return stream << value[0] << ", " << value[1] << ", " << value[2] << ", " << value[3];
}

#if VFC_CLANG
#pragma GCC diagnostic pop
#endif

template <typename T>
static double unpackSNorm(T value, int absMinVal = std::abs(std::numeric_limits<T>::min()),
	typename std::make_unsigned<T>::type range =
		std::numeric_limits<typename std::make_unsigned<T>::type>::max())
{
	using UnsignedT = typename std::make_unsigned<T>::type;

	// First convert to a [0, 1] value.
	auto remappedData = (static_cast<UnsignedT>(value) + static_cast<UnsignedT>(absMinVal)) &
		static_cast<UnsignedT>(range);
	double unpackedVal = static_cast<double>(remappedData)/static_cast<double>(range);
	// Then convert to a [-1, 1] value.
	return unpackedVal*2 - 1.0;
}

TEST(VertexValueTest, X8_UNorm)
{
	auto layout = vfc::ElementLayout::X8;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint8_t data = 0;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 0, 0, 1), value);

	std::uint8_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0xFF;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0x34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data)/0xFF, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X8_SNorm)
{
	auto layout = vfc::ElementLayout::X8;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	auto data = std::numeric_limits<std::int8_t>::min();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(-1, 0, 0, 1), value);

	std::int8_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = std::numeric_limits<std::int8_t>::max();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 34;
	value.fromData(&data, layout, type);

	EXPECT_LT(0, value[0]);
	EXPECT_EQ(vfc::VertexValue(unpackSNorm(data), 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X8_UInt)
{
	auto layout = vfc::ElementLayout::X8;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint8_t data = 0;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	std::uint8_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0xFF;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0x34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X8_SInt)
{
	auto layout = vfc::ElementLayout::X8;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int8_t data = std::numeric_limits<std::int8_t>::min();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	std::int8_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = std::numeric_limits<std::int8_t>::max();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X8Y8_UNorm)
{
	auto layout = vfc::ElementLayout::X8Y8;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint8_t data[2] = {0, 0xFF};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 1, 0, 1), value);

	std::uint8_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 0x34;
	data[1] = 0xAB;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0])/0xFF, static_cast<double>(data[1])/0xFF,
		0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8_SNorm)
{
	auto layout = vfc::ElementLayout::X8Y8;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int8_t data[2] = {std::numeric_limits<std::int8_t>::min(),
		std::numeric_limits<std::int8_t>::max()};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(-1, 1, 0, 1), value);

	std::int8_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 34;
	data[1] = -67;
	value.fromData(&data, layout, type);

	EXPECT_LT(0, value[0]);
	EXPECT_GT(0, value[1]);
	EXPECT_EQ(vfc::VertexValue(unpackSNorm(data[0]), unpackSNorm(data[1]), 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8_UInt)
{
	auto layout = vfc::ElementLayout::X8Y8;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint8_t data[2] = {0, 0xFF};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	std::uint8_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 0x34;
	data[1] = 0xAB;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8_SInt)
{
	auto layout = vfc::ElementLayout::X8Y8;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int8_t data[2] = {std::numeric_limits<std::int8_t>::min(),
		std::numeric_limits<std::int8_t>::max()};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	std::int8_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 34;
	data[1] = -67;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8Z8_UNorm)
{
	auto layout = vfc::ElementLayout::X8Y8Z8;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint8_t data[3] = {0, 0xFF, 0x34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 1, static_cast<double>(data[2])/0xFF, 1), value);

	std::uint8_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8Z8_SNorm)
{
	auto layout = vfc::ElementLayout::X8Y8Z8;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int8_t data[3] = {std::numeric_limits<std::int8_t>::min(),
		std::numeric_limits<std::int8_t>::max(), 34};
	value.fromData(&data, layout, type);

	EXPECT_LT(0, value[2]);
	EXPECT_EQ(vfc::VertexValue(-1, 1, unpackSNorm(data[2]), 1), value);

	std::int8_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8Z8_UInt)
{
	auto layout = vfc::ElementLayout::X8Y8Z8;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint8_t data[3] = {0, 0xFF, 0x34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], 1), value);

	std::uint8_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8Z8_SInt)
{
	auto layout = vfc::ElementLayout::X8Y8Z8;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int8_t data[3] = {std::numeric_limits<std::int8_t>::min(),
		std::numeric_limits<std::int8_t>::max(), 34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], 1), value);

	std::int8_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8Z8W8_UNorm)
{
	auto layout = vfc::ElementLayout::X8Y8Z8W8;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint8_t data[4] = {0, 0xFF, 0x34, 0xAB};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 1, static_cast<double>(data[2])/0xFF,
		static_cast<double>(data[3])/0xFF), value);

	std::uint8_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8Z8W8_SNorm)
{
	auto layout = vfc::ElementLayout::X8Y8Z8W8;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int8_t data[4] = {std::numeric_limits<std::int8_t>::min(),
		std::numeric_limits<std::int8_t>::max(), 34, -67};
	value.fromData(&data, layout, type);

	EXPECT_LT(0, value[2]);
	EXPECT_GT(0, value[3]);
	EXPECT_EQ(vfc::VertexValue(-1, 1, unpackSNorm(data[2]), unpackSNorm(data[3])), value);

	std::int8_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8Z8W8_UInt)
{
	auto layout = vfc::ElementLayout::X8Y8Z8W8;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint8_t data[4] = {0, 0xFF, 0x34, 0xAB};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], data[3]), value);

	std::uint8_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X8Y8Z8W8_SInt)
{
	auto layout = vfc::ElementLayout::X8Y8Z8W8;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int8_t data[4] = {std::numeric_limits<std::int8_t>::min(),
		std::numeric_limits<std::int8_t>::max(), 34, -67};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], data[3]), value);

	std::int8_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, W2X10Y10Z10_UNorm)
{
	auto layout = vfc::ElementLayout::W2X10Y10Z10;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	uint32_t data = (0 << 20) | (0x3FF << 10) | (0x34) | (0x1 << 30);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 1, static_cast<double>(0x34)/0x3FF, 1.0/3.0), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, W2X10Y10Z10_SNorm)
{
	auto layout = vfc::ElementLayout::W2X10Y10Z10;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	uint32_t data = (0x200 << 20) | (0x1FF << 10) | (0x234) | (0x2 << 30);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(-1, 1, unpackSNorm(0x234, 0x200, 0x3FF), -1), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, W2X10Y10Z10_UInt)
{
	auto layout = vfc::ElementLayout::W2X10Y10Z10;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	uint32_t data = (0 << 20) | (0x3FF << 10) | (0x34) | (0x1 << 30);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 0x3FF, 0x34, 0x1), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, W2X10Y10Z10_SInt)
{
	auto layout = vfc::ElementLayout::W2X10Y10Z10;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	uint32_t data = (0x200 << 20) | (0x1FF << 10) | (0x234) | (0x3 << 30);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<std::int16_t>(0xFE00), 0x1FF,
		static_cast<std::int16_t>(0xFE34), -1), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, W2Z10Y10X10_UNorm)
{
	auto layout = vfc::ElementLayout::W2Z10Y10X10;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	uint32_t data = (0) | (0x3FF << 10) | (0x34 << 20) | (0x1 << 30);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 1, static_cast<double>(0x34)/0x3FF, 1.0/3.0), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, W2Z10Y10X10_SNorm)
{
	auto layout = vfc::ElementLayout::W2Z10Y10X10;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	uint32_t data = (0x200) | (0x1FF << 10) | (0x234 << 20) | (0x2 << 30);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(-1, 1, unpackSNorm(0x234, 0x200, 0x3FF), -1), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, W2Z10Y10X10_UInt)
{
	auto layout = vfc::ElementLayout::W2Z10Y10X10;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	uint32_t data = (0) | (0x3FF << 10) | (0x34 << 20) | (0x1 << 30);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 0x3FF, 0x34, 0x1), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, W2Z10Y10X10_SInt)
{
	auto layout = vfc::ElementLayout::W2Z10Y10X10;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	uint32_t data = (0x200) | (0x1FF << 10) | (0x234 << 20) | (0x3 << 30);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<std::int16_t>(0xFE00), 0x1FF,
		static_cast<std::int16_t>(0xFE34), -1), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X16_UNorm)
{
	auto layout = vfc::ElementLayout::X16;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint16_t data = 0;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 0, 0, 1), value);

	std::uint16_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0xFFFF;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0x34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data)/0xFFFF, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X16_SNorm)
{
	auto layout = vfc::ElementLayout::X16;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	auto data = std::numeric_limits<std::int16_t>::min();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(-1, 0, 0, 1), value);

	std::int16_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = std::numeric_limits<std::int16_t>::max();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 34;
	value.fromData(&data, layout, type);

	EXPECT_LT(0, value[0]);
	EXPECT_EQ(vfc::VertexValue(unpackSNorm(data), 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X16_UInt)
{
	auto layout = vfc::ElementLayout::X16;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint16_t data = 0;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	std::uint16_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0xFFFF;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0x34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X16_SInt)
{
	auto layout = vfc::ElementLayout::X16;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int16_t data = std::numeric_limits<std::int16_t>::min();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	std::int16_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = std::numeric_limits<std::int16_t>::max();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X16_Float)
{
	auto layout = vfc::ElementLayout::X16;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int16_t data = glm::packHalf(glm::vec1(1.25f)).x;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, 0, 0, 1), value);

	std::uint16_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X16Y16_UNorm)
{
	auto layout = vfc::ElementLayout::X16Y16;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint16_t data[2] = {0, 0xFFFF};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 1, 0, 1), value);

	std::uint16_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 0x34;
	data[1] = 0xAB;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0])/0xFFFF, static_cast<double>(data[1])/0xFFFF,
		0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16_SNorm)
{
	auto layout = vfc::ElementLayout::X16Y16;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int16_t data[2] = {std::numeric_limits<std::int16_t>::min(),
		std::numeric_limits<std::int16_t>::max()};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(-1, 1, 0, 1), value);

	std::int16_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 34;
	data[1] = -67;
	value.fromData(&data, layout, type);

	EXPECT_LT(0, value[0]);
	EXPECT_GT(0, value[1]);
	EXPECT_EQ(vfc::VertexValue(unpackSNorm(data[0]), unpackSNorm(data[1]), 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16_UInt)
{
	auto layout = vfc::ElementLayout::X16Y16;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint16_t data[2] = {0, 0xFFFF};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	std::uint16_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 0x34;
	data[1] = 0xAB;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16_SInt)
{
	auto layout = vfc::ElementLayout::X16Y16;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int16_t data[2] = {std::numeric_limits<std::int16_t>::min(),
		std::numeric_limits<std::int16_t>::max()};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	std::int16_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 34;
	data[1] = -67;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16_Float)
{
	auto layout = vfc::ElementLayout::X16Y16;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	auto data = glm::packHalf(glm::vec2(1.25f, -2.0f));
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, -2.0, 0, 1), value);

	std::uint16_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(&data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16_UNorm)
{
	auto layout = vfc::ElementLayout::X16Y16Z16;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint16_t data[3] = {0, 0xFFFF, 0x34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 1, static_cast<double>(data[2])/0xFFFF, 1), value);

	std::uint16_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16_SNorm)
{
	auto layout = vfc::ElementLayout::X16Y16Z16;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int16_t data[3] = {std::numeric_limits<std::int16_t>::min(),
		std::numeric_limits<std::int16_t>::max(), 34};
	value.fromData(&data, layout, type);

	EXPECT_LT(0, value[2]);
	EXPECT_EQ(vfc::VertexValue(-1, 1, unpackSNorm(data[2]), 1), value);

	std::int16_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16_UInt)
{
	auto layout = vfc::ElementLayout::X16Y16Z16;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint16_t data[3] = {0, 0xFFFF, 0x34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], 1), value);

	std::uint16_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16_SInt)
{
	auto layout = vfc::ElementLayout::X16Y16Z16;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int16_t data[3] = {std::numeric_limits<std::int16_t>::min(),
		std::numeric_limits<std::int16_t>::max(), 34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], 1), value);

	std::int16_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16_Float)
{
	auto layout = vfc::ElementLayout::X16Y16Z16;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	auto data = glm::packHalf(glm::vec3(1.25f, -2.0f, 5.5f));
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, -2.0, 5.5, 1), value);

	std::uint16_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(&data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16W16_UNorm)
{
	auto layout = vfc::ElementLayout::X16Y16Z16W16;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint16_t data[4] = {0, 0xFFFF, 0x34, 0xAB};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(0, 1, static_cast<double>(data[2])/0xFFFF,
		static_cast<double>(data[3])/0xFFFF), value);

	std::uint16_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16W16_SNorm)
{
	auto layout = vfc::ElementLayout::X16Y16Z16W16;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int16_t data[4] = {std::numeric_limits<std::int16_t>::min(),
		std::numeric_limits<std::int16_t>::max(), 34, -67};
	value.fromData(&data, layout, type);

	EXPECT_LT(0, value[2]);
	EXPECT_GT(0, value[3]);
	EXPECT_EQ(vfc::VertexValue(-1, 1, unpackSNorm(data[2]), unpackSNorm(data[3])), value);

	std::int16_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16W16_UInt)
{
	auto layout = vfc::ElementLayout::X16Y16Z16W16;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint16_t data[4] = {0, 0xFFFF, 0x34, 0xAB};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], data[3]), value);

	std::uint16_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16W16_SInt)
{
	auto layout = vfc::ElementLayout::X16Y16Z16W16;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int16_t data[4] = {std::numeric_limits<std::int16_t>::min(),
		std::numeric_limits<std::int16_t>::max(), 34, -67};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], data[3]), value);

	std::int16_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X16Y16Z16W16_Float)
{
	auto layout = vfc::ElementLayout::X16Y16Z16W16;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	auto data = glm::packHalf(glm::vec4(1.25f, -2.0f, 5.5f, -10.0f));
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, -2.0, 5.5, -10.0), value);

	std::uint16_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(&data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X32_UInt)
{
	auto layout = vfc::ElementLayout::X32;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint32_t data = 0;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0xFFFFFFFF;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0x34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X32_SInt)
{
	auto layout = vfc::ElementLayout::X32;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int32_t data = std::numeric_limits<std::int32_t>::min();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	std::int32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = std::numeric_limits<std::int32_t>::max();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data, 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X32_Float)
{
	auto layout = vfc::ElementLayout::X32;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	float data = 1.25f;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, 0, 0, 1), value);

	float otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X32Y32_UInt)
{
	auto layout = vfc::ElementLayout::X32Y32;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint32_t data[2] = {0, 0xFFFFFFFF};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	std::uint32_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 0x34;
	data[1] = 0xAB;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X32Y32_SInt)
{
	auto layout = vfc::ElementLayout::X32Y32;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int32_t data[2] = {std::numeric_limits<std::int32_t>::min(),
		std::numeric_limits<std::int32_t>::max()};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	std::int32_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 34;
	data[1] = -67;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X32Y32_Float)
{
	auto layout = vfc::ElementLayout::X32Y32;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	float data[2] = {1.25f, -2.0f};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, -2.0, 0, 1), value);

	float otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(&data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X32Y32Z32_UInt)
{
	auto layout = vfc::ElementLayout::X32Y32Z32;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint32_t data[3] = {0, 0xFFFFFFFF, 0x34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], 1), value);

	std::uint32_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X32Y32Z32_SInt)
{
	auto layout = vfc::ElementLayout::X32Y32Z32;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int32_t data[3] = {std::numeric_limits<std::int32_t>::min(),
		std::numeric_limits<std::int32_t>::max(), 34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], 1), value);

	std::int32_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X32Y32Z32_Float)
{
	auto layout = vfc::ElementLayout::X32Y32Z32;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	float data[3] = {1.25f, -2.0f, 5.5f};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, -2.0, 5.5, 1), value);

	float otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(&data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X32Y32Z32W32_UInt)
{
	auto layout = vfc::ElementLayout::X32Y32Z32W32;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint32_t data[4] = {0, 0xFFFFFFFF, 0x34, 0xAB};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], data[3]), value);

	std::uint32_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X32Y32Z32W32_SInt)
{
	auto layout = vfc::ElementLayout::X32Y32Z32W32;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int32_t data[4] = {std::numeric_limits<std::int32_t>::min(),
		std::numeric_limits<std::int32_t>::max(), 34, -67};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(data[0], data[1], data[2], data[3]), value);

	std::int32_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X32Y32Z32W32_Float)
{
	auto layout = vfc::ElementLayout::X32Y32Z32W32;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	float data[4] = {1.25f, -2.0f, 5.5f, -10.0f};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, -2.0, 5.5, -10.0), value);

	float otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(&data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X64_UInt)
{
	auto layout = vfc::ElementLayout::X64;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint64_t data = 0;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data), 0, 0, 1), value);

	std::uint64_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0xFFFFFFFF;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data), 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 0x34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data), 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X64_SInt)
{
	auto layout = vfc::ElementLayout::X64;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int64_t data = std::numeric_limits<std::int32_t>::min();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data), 0, 0, 1), value);

	std::int64_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = std::numeric_limits<std::int32_t>::max();
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data), 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);

	data = 34;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data), 0, 0, 1), value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X64_Float)
{
	auto layout = vfc::ElementLayout::X64;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	double data = 1.25;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, 0, 0, 1), value);

	double otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, X64Y64_UInt)
{
	auto layout = vfc::ElementLayout::X64Y64;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint64_t data[2] = {0, 0xFFFFFFFF};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0]), static_cast<double>(data[1]),
		0, 1), value);

	std::uint64_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 0x34;
	data[1] = 0xAB;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0]), static_cast<double>(data[1]), 0, 1),
		value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X64Y64_SInt)
{
	auto layout = vfc::ElementLayout::X64Y64;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int64_t data[2] = {std::numeric_limits<std::int32_t>::min(),
		std::numeric_limits<std::int32_t>::max()};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0]), static_cast<double>(data[1]), 0, 1),
		value);

	std::int64_t otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));

	data[0] = 34;
	data[1] = -67;
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0]), static_cast<double>(data[1]), 0, 1),
		value);

	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X64Y64_Float)
{
	auto layout = vfc::ElementLayout::X64Y64;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	double data[2] = {1.25, -2.0};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, -2.0, 0, 1), value);

	double otherData[2];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(&data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X64Y64Z64_UInt)
{
	auto layout = vfc::ElementLayout::X64Y64Z64;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint64_t data[3] = {0, 0xFFFFFFFF, 0x34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0]), static_cast<double>(data[1]),
		static_cast<double>(data[2]), 1), value);

	std::uint64_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X64Y64Z64_SInt)
{
	auto layout = vfc::ElementLayout::X64Y64Z64;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int64_t data[3] = {std::numeric_limits<std::int32_t>::min(),
		std::numeric_limits<std::int32_t>::max(), 34};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0]), static_cast<double>(data[1]),
		static_cast<double>(data[2]), 1), value);

	std::int64_t otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X64Y64Z64_Float)
{
	auto layout = vfc::ElementLayout::X64Y64Z64;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	double data[3] = {1.25, -2.0, 5.5};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, -2.0, 5.5, 1), value);

	double otherData[3];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(&data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X64Y64Z64W64_UInt)
{
	auto layout = vfc::ElementLayout::X64Y64Z64W64;
	auto type = vfc::ElementType::UInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::uint64_t data[4] = {0, 0xFFFFFFFF, 0x34, 0xAB};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0]), static_cast<double>(data[1]),
		static_cast<double>(data[2]), static_cast<double>(data[3])), value);

	std::uint64_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X64Y64Z64W64_SInt)
{
	auto layout = vfc::ElementLayout::X64Y64Z64W64;
	auto type = vfc::ElementType::SInt;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	std::int64_t data[4] = {std::numeric_limits<std::int32_t>::min(),
		std::numeric_limits<std::int32_t>::max(), 34, -67};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(static_cast<double>(data[0]), static_cast<double>(data[1]),
		static_cast<double>(data[2]), static_cast<double>(data[3])), value);

	std::int64_t otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(data, otherData, sizeof(data)));
}

TEST(VertexValueTest, X64Y64Z64W64_Float)
{
	auto layout = vfc::ElementLayout::X64Y64Z64W64;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	double data[4] = {1.25, -2.0, 5.5, -10.0};
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, -2.0, 5.5, -10.0), value);

	double otherData[4];
	value.toData(&otherData, layout, type);
	EXPECT_EQ(0, std::memcmp(&data, otherData, sizeof(data)));
}

TEST(VertexValueTest, Z10Y11X11_UFloat)
{
	auto layout = vfc::ElementLayout::Z10Y11X11_UFloat;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	glm::vec3 baseValues(1.25, 2.0, 5.5);
	std::uint32_t data = glm::packF2x11_1x10(baseValues);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, 2.0, 5.5, 1), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, E5Z9Y9X9_UFloat)
{
	auto layout = vfc::ElementLayout::E5Z9Y9X9_UFloat;
	auto type = vfc::ElementType::Float;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue value(1, 2, 3, 4);
	glm::vec3 baseValues(1.25, 2.0, 5.5);
	std::uint32_t data = glm::packF3x9_E1x5(baseValues);
	value.fromData(&data, layout, type);

	EXPECT_EQ(vfc::VertexValue(1.25, 2.0, 5.5, 1), value);

	std::uint32_t otherData;
	value.toData(&otherData, layout, type);
	EXPECT_EQ(data, otherData);
}

TEST(VertexValueTest, PackBoundedUNorm)
{
	auto layout = vfc::ElementLayout::X8Y8Z8W8;
	auto type = vfc::ElementType::UNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue minVal(-0.1, -0.2, -0.3, -0.4);
	vfc::VertexValue maxVal(0.1, 0.2, 0.3, 0.4);

	std::uint8_t data[4];
	minVal.toData(data, layout, type, minVal, maxVal);
	vfc::VertexValue tempValue;
	tempValue.fromData(data, layout, type);
	EXPECT_EQ(vfc::VertexValue(0, 0, 0, 0), tempValue);

	maxVal.toData(data, layout, type, minVal, maxVal);
	tempValue.fromData(data, layout, type);
	EXPECT_EQ(vfc::VertexValue(1, 1, 1, 1), tempValue);
}

TEST(VertexValueTest, PackBoundedSNorm)
{
	auto layout = vfc::ElementLayout::X8Y8Z8W8;
	auto type = vfc::ElementType::SNorm;
	ASSERT_TRUE(vfc::isElementValid(layout, type));

	vfc::VertexValue minVal(-0.1, -0.2, -0.3, -0.4);
	vfc::VertexValue maxVal(0.1, 0.2, 0.3, 0.4);

	std::uint8_t data[4];
	minVal.toData(data, layout, type, minVal, maxVal);
	vfc::VertexValue tempValue;
	tempValue.fromData(data, layout, type);
	EXPECT_EQ(vfc::VertexValue(-1, -1, -1, -1), tempValue);

	maxVal.toData(data, layout, type, minVal, maxVal);
	tempValue.fromData(data, layout, type);
	EXPECT_EQ(vfc::VertexValue(1, 1, 1, 1), tempValue);
}
