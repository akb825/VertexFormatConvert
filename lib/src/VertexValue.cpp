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

#include <VFC/VertexValue.h>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

#if VFC_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <glm/gtc/packing.hpp>
#include <glm/glm.hpp>

#if VFC_GCC
#pragma GCC diagnostic pop
#endif

namespace vfc
{

namespace
{

// Constants for 2 10 10 10 packed values
constexpr std::uint32_t mask10 = 0x3FF;
constexpr std::uint32_t mask2 = 0x3;
constexpr std::uint32_t absMinValue10 = 0x200;
constexpr std::uint32_t absMinValue2 = 0x2;
constexpr std::int32_t minValue10 = -static_cast<std::int32_t>(absMinValue10);
constexpr std::int32_t minValue2 = -static_cast<std::int32_t>(absMinValue2);
constexpr std::int32_t maxSignedValue10 = 0x1FF;
constexpr std::int32_t maxSignedValue2 = 0x1;

template <typename T>
inline double unpackUNorm(T data, T maxValue)
{
	return static_cast<double>(data)/static_cast<double>(maxValue);
}

inline void unpackUNormValues(double* values, const std::uint8_t* data, unsigned int count)
{
	constexpr auto maxValue = std::numeric_limits<std::uint8_t>::max();
	for (unsigned int i = 0; i < count; ++i)
		values[i] = unpackUNorm(data[i], maxValue);
}

template <typename T>
inline void unpackUNormValues(double* values, const T* data, unsigned int count)
{
	constexpr auto maxValue = std::numeric_limits<T>::max();

	assert(count <= 4);
	T alignedData[4];
	std::memcpy(alignedData, data, sizeof(T)*count);
	for (unsigned int i = 0; i < count; ++i)
		values[i] = unpackUNorm(alignedData[i], maxValue);
}

template <typename T, typename U, typename V>
inline double unpackSNorm(T data, U absMinValue, V range)
{
	using UnsignedT = typename std::make_unsigned<T>::type;

	// First convert to a [0, 1] value.
	auto remappedData = (static_cast<UnsignedT>(data) + static_cast<UnsignedT>(absMinValue)) &
		static_cast<UnsignedT>(range);
	double value = static_cast<double>(remappedData)/static_cast<double>(range);
	// Then convert to a [-1, 1] value.
	return value*2 - 1.0;
}

inline void unpackSNormValues(double* values, const std::int8_t* data, unsigned int count)
{
	const auto absMinValue = std::abs(std::numeric_limits<std::int8_t>::min());
	constexpr auto range = std::numeric_limits<std::uint8_t>::max();

	for (unsigned int i = 0; i < count; ++i)
		values[i] = unpackSNorm(data[i], absMinValue, range);
}

template <typename T>
inline void unpackSNormValues(double* values, const T* data, unsigned int count)
{
	const auto absMinValue = std::abs(std::numeric_limits<T>::min());
	constexpr auto range = std::numeric_limits<typename std::make_unsigned<T>::type>::max();

	assert(count <= 4);
	T alignedData[4];
	std::memcpy(alignedData, data, sizeof(T)*count);
	for (unsigned int i = 0; i < count; ++i)
		values[i] = unpackSNorm(alignedData[i], absMinValue, range);
}

inline void unpackDirect(double* values, const std::uint8_t* data, unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
		values[i] = static_cast<double>(data[i]);
}

inline void unpackDirect(double* values, const std::int8_t* data, unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
		values[i] = static_cast<double>(data[i]);
}

template <typename T>
inline void unpackDirect(double* values, const T* data, unsigned int count)
{
	assert(count <= 4);
	T alignedData[4];
	std::memcpy(alignedData, data, sizeof(T)*count);
	for (unsigned int i = 0; i < count; ++i)
		values[i] = static_cast<double>(alignedData[i]);
}

inline void unpackHalfFloatValues(double* values, const std::uint16_t* data, unsigned int count)
{
	assert(count <= 4);
	std::uint16_t alignedData[4];
	std::memcpy(alignedData, data, sizeof(std::uint16_t)*count);
	for (unsigned int i = 0; i < count; ++i)
		values[i] = glm::unpackHalf(glm::u16vec1(alignedData[i])).x;
}

template <typename T>
inline typename std::make_signed<T>::type makeSigned(T value, T signBit)
{
	using SignedT = typename std::make_signed<T>::type;
	return static_cast<SignedT>((value ^ signBit) - signBit);
}

template <typename T>
inline T packUNorm(double value, T maxValue)
{
	return static_cast<T>(std::round(glm::clamp(value, 0.0, 1.0)*static_cast<double>(maxValue)));
}

inline void packUNormValues(std::uint8_t* data, const double* values, unsigned int count)
{
	constexpr auto maxValue = std::numeric_limits<std::uint8_t>::max();
	for (unsigned int i = 0; i < count; ++i)
		data[i] = packUNorm(values[i], maxValue);
}

template <typename T>
inline void packUNormValues(T* data, const double* values, unsigned int count)
{
	constexpr auto maxValue = std::numeric_limits<T>::max();

	assert(count <= 4);
	T alignedData[4];
	for (unsigned int i = 0; i < count; ++i)
		alignedData[i] = packUNorm(values[i], maxValue);
	std::memcpy(data, alignedData, sizeof(T)*count);
}

template <typename T, typename U, typename V>
inline T packSNorm(double value, U absMinValue, V range)
{
	using UnsignedT = typename std::make_unsigned<T>::type;
	double unormVal = glm::clamp(value, -1.0, 1.0)*0.5 + 0.5;
	auto baseRange = static_cast<UnsignedT>(std::round(unormVal*static_cast<double>(range)));
	return static_cast<T>(baseRange - absMinValue);
}

inline void packSNormValues(std::int8_t* data, const double* values, unsigned int count)
{
	const auto absMinValue = std::abs(std::numeric_limits<std::int8_t>::min());
	constexpr auto range = std::numeric_limits<std::uint8_t>::max();

	for (unsigned int i = 0; i < count; ++i)
		data[i] = packSNorm<std::int8_t>(values[i], absMinValue, range);
}

template <typename T>
inline void packSNormValues(T* data, const double* values, unsigned int count)
{
	const auto absMinValue = std::abs(std::numeric_limits<T>::min());
	constexpr auto range = std::numeric_limits<typename std::make_unsigned<T>::type>::max();

	assert(count <= 4);
	T alignedData[4];
	for (unsigned int i = 0; i < count; ++i)
		alignedData[i] = packSNorm<T>(values[i], absMinValue, range);
	std::memcpy(data, alignedData, sizeof(T)*count);
}

template <typename T>
inline T packInteger(double value, T minVal, T maxVal)
{
	return static_cast<T>(std::round(glm::clamp(value, static_cast<double>(minVal),
		static_cast<double>(maxVal))));
}

inline void packIntegers(std::uint8_t* data, const double* values, unsigned int count)
{
	constexpr auto minValue = std::numeric_limits<std::uint8_t>::min();
	constexpr auto maxValue = std::numeric_limits<std::uint8_t>::max();

	for (unsigned int i = 0; i < count; ++i)
		data[i] = packInteger(values[i], minValue, maxValue);
}

inline void packIntegers(std::int8_t* data, const double* values, unsigned int count)
{
	constexpr auto minValue = std::numeric_limits<std::int8_t>::min();
	constexpr auto maxValue = std::numeric_limits<std::int8_t>::max();

	for (unsigned int i = 0; i < count; ++i)
		data[i] = packInteger(values[i], minValue, maxValue);
}

template <typename T>
inline void packIntegers(T* data, const double* values, unsigned int count)
{
	constexpr auto minValue = std::numeric_limits<T>::min();
	constexpr auto maxValue = std::numeric_limits<T>::max();

	assert(count <= 4);
	T alignedData[4];
	for (unsigned int i = 0; i < count; ++i)
		alignedData[i] = packInteger(values[i], minValue, maxValue);
	std::memcpy(data, alignedData, sizeof(T)*count);
}

template <typename T>
inline void packDirect(T* data, const double* values, unsigned int count)
{
	assert(count <= 4);
	T alignedData[4];
	for (unsigned int i = 0; i < count; ++i)
		alignedData[i] = static_cast<T>(values[i]);
	std::memcpy(data, alignedData, sizeof(T)*count);
}

inline void packHalfFloatValues(std::uint16_t* data, const double* values, unsigned int count)
{
	assert(count <= 4);
	std::uint16_t alignedData[4];
	for (unsigned int i = 0; i < count; ++i)
		alignedData[i] = glm::packHalf(glm::vec1(static_cast<float>(values[i]))).x;
	std::memcpy(data, alignedData, sizeof(std::uint16_t)*count);
}

} // namespace

constexpr unsigned int VertexValue::count;

const VertexValue VertexValue::initialBoundsMin(std::numeric_limits<double>::max(),
	std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
	std::numeric_limits<double>::max());

const VertexValue VertexValue::initialBoundsMax(-std::numeric_limits<double>::max(),
	-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(),
	-std::numeric_limits<double>::max());

bool VertexValue::fromData(const void* data, ElementLayout layout, ElementType type)
{
	if (!data)
		return false;

	switch (layout)
	{
		case ElementLayout::X8:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint8_t*>(data), 1);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int8_t*>(data), 1);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint8_t*>(data), 1);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int8_t*>(data), 1);
					break;
				default:
					return false;
			}
			m_values[1] = 0;
			m_values[2] = 0;
			m_values[3] = 1;
			return true;
		case ElementLayout::X8Y8:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint8_t*>(data), 2);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int8_t*>(data), 2);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint8_t*>(data), 2);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int8_t*>(data), 2);
					break;
				default:
					return false;
			}
			m_values[2] = 0;
			m_values[3] = 1;
			return true;
		case ElementLayout::X8Y8Z8:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint8_t*>(data), 3);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int8_t*>(data), 3);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint8_t*>(data), 3);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int8_t*>(data), 3);
					break;
				default:
					return false;
			}
			m_values[3] = 1;
			return true;
		case ElementLayout::X8Y8Z8W8:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint8_t*>(data), 4);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int8_t*>(data), 4);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint8_t*>(data), 4);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int8_t*>(data), 4);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::W2X10Y10Z10:
		{
			std::uint32_t dataValue;
			std::memcpy(&dataValue, reinterpret_cast<const std::uint32_t*>(data),
				sizeof(std::uint32_t));
			switch (type)
			{
				case ElementType::UNorm:
					m_values[0] = unpackUNorm((dataValue >> 20) & mask10, mask10);
					m_values[1] = unpackUNorm((dataValue >> 10) & mask10, mask10);
					m_values[2] = unpackUNorm(dataValue & mask10, mask10);
					m_values[3] = unpackUNorm((dataValue >> 30) & mask2, mask2);
					break;
				case ElementType::SNorm:
					m_values[0] = unpackSNorm((dataValue >> 20) & mask10, absMinValue10, mask10);
					m_values[1] = unpackSNorm((dataValue >> 10) & mask10, absMinValue10, mask10);
					m_values[2] = unpackSNorm(dataValue & mask10, absMinValue10, mask10);
					m_values[3] = unpackSNorm((dataValue >> 30) & mask2, absMinValue2, mask2);
					break;
				case ElementType::UInt:
					m_values[0] = (dataValue >> 20) & mask10;
					m_values[1] = (dataValue >> 10) & mask10;
					m_values[2] = dataValue & mask10;
					m_values[3] = (dataValue >> 30) & mask2;
					break;
				case ElementType::SInt:
					m_values[0] = makeSigned((dataValue >> 20) & mask10, absMinValue10);
					m_values[1] = makeSigned((dataValue >> 10) & mask10, absMinValue10);
					m_values[2] = makeSigned(dataValue & mask10, absMinValue10);
					m_values[3] = makeSigned((dataValue >> 30) & mask2, absMinValue2);
					break;
				default:
					return false;
			}
			return true;
		}
		case ElementLayout::W2Z10Y10X10:
		{
			std::uint32_t dataValue;
			std::memcpy(&dataValue, reinterpret_cast<const std::uint32_t*>(data),
				sizeof(std::uint32_t));
			switch (type)
			{
				case ElementType::UNorm:
					m_values[0] = unpackUNorm(dataValue & mask10, mask10);
					m_values[1] = unpackUNorm((dataValue >> 10) & mask10, mask10);
					m_values[2] = unpackUNorm((dataValue >> 20) & mask10, mask10);
					m_values[3] = unpackUNorm((dataValue >> 30) & mask2, mask2);
					break;
				case ElementType::SNorm:
					m_values[0] = unpackSNorm(dataValue & mask10, absMinValue10, mask10);
					m_values[1] = unpackSNorm((dataValue >> 10) & mask10, absMinValue10, mask10);
					m_values[2] = unpackSNorm((dataValue >> 20) & mask10, absMinValue10, mask10);
					m_values[3] = unpackSNorm((dataValue >> 30) & mask2, absMinValue2, mask2);
					break;
				case ElementType::UInt:
					m_values[0] = dataValue & mask10;
					m_values[1] = (dataValue >> 10) & mask10;
					m_values[2] = (dataValue >> 20) & mask10;
					m_values[3] = (dataValue >> 30) & mask2;
					break;
				case ElementType::SInt:
					m_values[0] = makeSigned(dataValue & mask10, absMinValue10);
					m_values[1] = makeSigned((dataValue >> 10) & mask10, absMinValue10);
					m_values[2] = makeSigned((dataValue >> 20) & mask10, absMinValue10);
					m_values[3] = makeSigned((dataValue >> 30) & mask2, absMinValue2);
					break;
				default:
					return false;
			}
			return true;
		}
		case ElementLayout::X16:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint16_t*>(data), 1);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int16_t*>(data), 1);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint16_t*>(data), 1);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int16_t*>(data), 1);
					break;
				case ElementType::Float:
					unpackHalfFloatValues(m_values, reinterpret_cast<const std::uint16_t*>(data), 1);
					break;
				default:
					return false;
			}
			m_values[1] = 0;
			m_values[2] = 0;
			m_values[3] = 1;
			return true;
		case ElementLayout::X16Y16:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint16_t*>(data), 2);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int16_t*>(data), 2);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint16_t*>(data), 2);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int16_t*>(data), 2);
					break;
				case ElementType::Float:
					unpackHalfFloatValues(m_values, reinterpret_cast<const std::uint16_t*>(data), 2);
					break;
				default:
					return false;
			}
			m_values[2] = 0;
			m_values[3] = 1;
			return true;
		case ElementLayout::X16Y16Z16:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint16_t*>(data), 3);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int16_t*>(data), 3);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint16_t*>(data), 3);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int16_t*>(data), 3);
					break;
				case ElementType::Float:
					unpackHalfFloatValues(m_values, reinterpret_cast<const std::uint16_t*>(data), 3);
					break;
				default:
					return false;
			}
			m_values[3] = 1;
			return true;
		case ElementLayout::X16Y16Z16W16:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint16_t*>(data), 4);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int16_t*>(data), 4);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint16_t*>(data), 4);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int16_t*>(data), 4);
					break;
				case ElementType::Float:
					unpackHalfFloatValues(m_values, reinterpret_cast<const std::uint16_t*>(data), 4);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X32:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint32_t*>(data), 1);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int32_t*>(data), 1);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint32_t*>(data), 1);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int32_t*>(data), 1);
					break;
				case ElementType::Float:
					unpackDirect(m_values, reinterpret_cast<const float*>(data), 1);
					break;
				default:
					return false;
			}
			m_values[1] = 0;
			m_values[2] = 0;
			m_values[3] = 1;
			return true;
		case ElementLayout::X32Y32:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint32_t*>(data), 2);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int32_t*>(data), 2);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint32_t*>(data), 2);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int32_t*>(data), 2);
					break;
				case ElementType::Float:
					unpackDirect(m_values, reinterpret_cast<const float*>(data), 2);
					break;
				default:
					return false;
			}
			m_values[2] = 0;
			m_values[3] = 1;
			return true;
		case ElementLayout::X32Y32Z32:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint32_t*>(data), 3);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int32_t*>(data), 3);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint32_t*>(data), 3);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int32_t*>(data), 3);
					break;
				case ElementType::Float:
					unpackDirect(m_values, reinterpret_cast<const float*>(data), 3);
					break;
				default:
					return false;
			}
			m_values[3] = 1;
			return true;
		case ElementLayout::X32Y32Z32W32:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint32_t*>(data), 4);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int32_t*>(data), 4);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint32_t*>(data), 4);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int32_t*>(data), 4);
					break;
				case ElementType::Float:
					unpackDirect(m_values, reinterpret_cast<const float*>(data), 4);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X64:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint64_t*>(data), 1);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int64_t*>(data), 1);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint64_t*>(data), 1);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int64_t*>(data), 1);
					break;
				case ElementType::Float:
					unpackDirect(m_values, reinterpret_cast<const double*>(data), 1);
					break;
				default:
					return false;
			}
			m_values[1] = 0;
			m_values[2] = 0;
			m_values[3] = 1;
			return true;
		case ElementLayout::X64Y64:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint64_t*>(data), 2);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int64_t*>(data), 2);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint64_t*>(data), 2);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int64_t*>(data), 2);
					break;
				case ElementType::Float:
					unpackDirect(m_values, reinterpret_cast<const double*>(data), 2);
					break;
				default:
					return false;
			}
			m_values[2] = 0;
			m_values[3] = 1;
			return true;
		case ElementLayout::X64Y64Z64:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint64_t*>(data), 3);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int64_t*>(data), 3);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint64_t*>(data), 3);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int64_t*>(data), 3);
					break;
				case ElementType::Float:
					unpackDirect(m_values, reinterpret_cast<const double*>(data), 3);
					break;
				default:
					return false;
			}
			m_values[3] = 1;
			return true;
		case ElementLayout::X64Y64Z64W64:
			switch (type)
			{
				case ElementType::UNorm:
					unpackUNormValues(m_values, reinterpret_cast<const std::uint64_t*>(data), 4);
					break;
				case ElementType::SNorm:
					unpackSNormValues(m_values, reinterpret_cast<const std::int64_t*>(data), 4);
					break;
				case ElementType::UInt:
					unpackDirect(m_values, reinterpret_cast<const std::uint64_t*>(data), 4);
					break;
				case ElementType::SInt:
					unpackDirect(m_values, reinterpret_cast<const std::int64_t*>(data), 4);
					break;
				case ElementType::Float:
					unpackDirect(m_values, reinterpret_cast<const double*>(data), 4);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::Z10Y11X11_UFloat:
			if (type == ElementType::Float)
			{
				std::uint32_t dataValue;
				std::memcpy(&dataValue, reinterpret_cast<const std::uint32_t*>(data),
					sizeof(std::uint32_t));
				glm::vec3 value = glm::unpackF2x11_1x10(dataValue);
				m_values[0] = value[0];
				m_values[1] = value[1];
				m_values[2] = value[2];
				m_values[3] = 1;
				return true;
			}
			else
				return false;
		case ElementLayout::E5Z9Y9X9_UFloat:
			if (type == ElementType::Float)
			{
				std::uint32_t dataValue;
				std::memcpy(&dataValue, reinterpret_cast<const std::uint32_t*>(data),
					sizeof(std::uint32_t));
				glm::vec3 value = glm::unpackF3x9_E1x5(dataValue);
				m_values[0] = value[0];
				m_values[1] = value[1];
				m_values[2] = value[2];
				m_values[3] = 1;
				return true;
			}
			else
				return false;
		default:
			return false;
	}
}

bool VertexValue::toData(void* outData, ElementLayout layout, ElementType type) const
{
	if (!outData)
		return false;

	switch (layout)
	{
		case ElementLayout::X8:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint8_t*>(outData), m_values, 1);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int8_t*>(outData), m_values, 1);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint8_t*>(outData), m_values, 1);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int8_t*>(outData), m_values, 1);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X8Y8:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint8_t*>(outData), m_values, 2);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int8_t*>(outData), m_values, 2);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint8_t*>(outData), m_values, 2);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int8_t*>(outData), m_values, 2);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X8Y8Z8:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint8_t*>(outData), m_values, 3);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int8_t*>(outData), m_values, 3);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint8_t*>(outData), m_values, 3);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int8_t*>(outData), m_values, 3);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X8Y8Z8W8:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint8_t*>(outData), m_values, 4);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int8_t*>(outData), m_values, 4);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint8_t*>(outData), m_values, 4);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int8_t*>(outData), m_values, 4);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::W2X10Y10Z10:
		{
			std::uint32_t outDataValue;
			switch (type)
			{
				case ElementType::UNorm:
					outDataValue = (packUNorm(m_values[0], mask10) << 20) |
						(packUNorm(m_values[1], mask10) << 10) | packUNorm(m_values[2], mask10) |
						(packUNorm(m_values[3], mask2) << 30);
					break;
				case ElementType::SNorm:
					outDataValue =
						((packSNorm<std::uint32_t>(m_values[0], absMinValue10, mask10) &
							mask10) << 20) |
						((packSNorm<std::uint32_t>(m_values[1], absMinValue10, mask10) &
							mask10) << 10) |
						(packSNorm<std::uint32_t>(m_values[2], absMinValue10, mask10) & mask10) |
						((packSNorm<std::uint32_t>(m_values[3], absMinValue2, mask2) &
							mask2) << 30);
					break;
				case ElementType::UInt:
					outDataValue = (packInteger(m_values[0], 0U, mask10) << 20) |
						(packInteger(m_values[1], 0U, mask10) << 10) |
						packInteger(m_values[2], 0U, mask10) |
						(packInteger(m_values[3], 0U, mask2) << 30);
					break;
				case ElementType::SInt:
					outDataValue =
						((packInteger(m_values[0], minValue10, maxSignedValue10) & mask10) << 20) |
						((packInteger(m_values[1], minValue10, maxSignedValue10) & mask10) << 10) |
						(packInteger(m_values[2], minValue10, maxSignedValue10) & mask10) |
						((packInteger(m_values[3], minValue2, maxSignedValue2) & mask2) << 30);
					break;
				default:
					return false;
			}
			std::memcpy(reinterpret_cast<std::uint32_t*>(outData), &outDataValue,
				sizeof(std::uint32_t));
			return true;
		}
		case ElementLayout::W2Z10Y10X10:
		{
			std::uint32_t outDataValue;
			switch (type)
			{
				case ElementType::UNorm:
					outDataValue = packUNorm(m_values[0], mask10) |
						(packUNorm(m_values[1], mask10) << 10) |
						(packUNorm(m_values[2], mask10) << 20) |
						(packUNorm(m_values[3], mask2) << 30);
					break;
				case ElementType::SNorm:
					outDataValue =
						(packSNorm<std::uint32_t>(m_values[0], absMinValue10, mask10) & mask10) |
						((packSNorm<std::uint32_t>(m_values[1], absMinValue10, mask10) &
							mask10) << 10) |
						((packSNorm<std::uint32_t>(m_values[2], absMinValue10, mask10) &
							mask10) << 20) |
						((packSNorm<std::uint32_t>(m_values[3], absMinValue2, mask2) &
							mask2) << 30);
					break;
				case ElementType::UInt:
					outDataValue = packInteger(m_values[0], 0U, mask10) |
						(packInteger(m_values[1], 0U, mask10) << 10) |
						(packInteger(m_values[2], 0U, mask10) << 20) |
						(packInteger(m_values[3], 0U, mask2) << 30);
					break;
				case ElementType::SInt:
					outDataValue =
						(packInteger(m_values[0], minValue10, maxSignedValue10) & mask10) |
						((packInteger(m_values[1], minValue10, maxSignedValue10) & mask10) << 10) |
						((packInteger(m_values[2], minValue10, maxSignedValue10) & mask10) << 20) |
						((packInteger(m_values[3], minValue2, maxSignedValue2) & mask2) << 30);
					break;
				default:
					return false;
			}
			std::memcpy(reinterpret_cast<std::uint32_t*>(outData), &outDataValue,
				sizeof(std::uint32_t));
			return true;
		}
		case ElementLayout::X16:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint16_t*>(outData), m_values, 1);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int16_t*>(outData), m_values, 1);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint16_t*>(outData), m_values, 1);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int16_t*>(outData), m_values, 1);
					break;
				case ElementType::Float:
					packHalfFloatValues(reinterpret_cast<std::uint16_t*>(outData), m_values, 1);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X16Y16:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint16_t*>(outData), m_values, 2);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int16_t*>(outData), m_values, 2);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint16_t*>(outData), m_values, 2);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int16_t*>(outData), m_values, 2);
					break;
				case ElementType::Float:
					packHalfFloatValues(reinterpret_cast<std::uint16_t*>(outData), m_values, 2);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X16Y16Z16:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint16_t*>(outData), m_values, 3);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int16_t*>(outData), m_values, 3);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint16_t*>(outData), m_values, 3);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int16_t*>(outData), m_values, 3);
					break;
				case ElementType::Float:
					packHalfFloatValues(reinterpret_cast<std::uint16_t*>(outData), m_values, 3);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X16Y16Z16W16:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint16_t*>(outData), m_values, 4);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int16_t*>(outData), m_values, 4);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint16_t*>(outData), m_values, 4);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int16_t*>(outData), m_values, 4);
					break;
				case ElementType::Float:
					packHalfFloatValues(reinterpret_cast<std::uint16_t*>(outData), m_values, 4);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X32:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint32_t*>(outData), m_values, 1);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int32_t*>(outData), m_values, 1);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint32_t*>(outData), m_values, 1);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int32_t*>(outData), m_values, 1);
					break;
				case ElementType::Float:
					packDirect(reinterpret_cast<float*>(outData), m_values, 1);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X32Y32:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint32_t*>(outData), m_values, 2);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int32_t*>(outData), m_values, 2);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint32_t*>(outData), m_values, 2);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int32_t*>(outData), m_values, 2);
					break;
				case ElementType::Float:
					packDirect(reinterpret_cast<float*>(outData), m_values, 2);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X32Y32Z32:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint32_t*>(outData), m_values, 3);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int32_t*>(outData), m_values, 3);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint32_t*>(outData), m_values, 3);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int32_t*>(outData), m_values, 3);
					break;
				case ElementType::Float:
					packDirect(reinterpret_cast<float*>(outData), m_values, 3);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X32Y32Z32W32:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint32_t*>(outData), m_values, 4);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int32_t*>(outData), m_values, 4);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint32_t*>(outData), m_values, 4);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int32_t*>(outData), m_values, 4);
					break;
				case ElementType::Float:
					packDirect(reinterpret_cast<float*>(outData), m_values, 4);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X64:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint64_t*>(outData), m_values, 1);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int64_t*>(outData), m_values, 1);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint64_t*>(outData), m_values, 1);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int64_t*>(outData), m_values, 1);
					break;
				case ElementType::Float:
					packDirect(reinterpret_cast<double*>(outData), m_values, 1);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X64Y64:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint64_t*>(outData), m_values, 2);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int64_t*>(outData), m_values, 2);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint64_t*>(outData), m_values, 2);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int64_t*>(outData), m_values, 2);
					break;
				case ElementType::Float:
					packDirect(reinterpret_cast<double*>(outData), m_values, 2);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X64Y64Z64:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint64_t*>(outData), m_values, 3);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int64_t*>(outData), m_values, 3);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint64_t*>(outData), m_values, 3);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int64_t*>(outData), m_values, 3);
					break;
				case ElementType::Float:
					packDirect(reinterpret_cast<double*>(outData), m_values, 3);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::X64Y64Z64W64:
			switch (type)
			{
				case ElementType::UNorm:
					packUNormValues(reinterpret_cast<std::uint64_t*>(outData), m_values, 4);
					break;
				case ElementType::SNorm:
					packSNormValues(reinterpret_cast<std::int64_t*>(outData), m_values, 4);
					break;
				case ElementType::UInt:
					packIntegers(reinterpret_cast<std::uint64_t*>(outData), m_values, 4);
					break;
				case ElementType::SInt:
					packIntegers(reinterpret_cast<std::int64_t*>(outData), m_values, 4);
					break;
				case ElementType::Float:
					packDirect(reinterpret_cast<double*>(outData), m_values, 4);
					break;
				default:
					return false;
			}
			return true;
		case ElementLayout::Z10Y11X11_UFloat:
			if (type == ElementType::Float)
			{
				std::uint32_t outDataValue =
					glm::packF2x11_1x10(glm::vec3(m_values[0], m_values[1], m_values[2]));
				std::memcpy(reinterpret_cast<std::uint32_t*>(outData), &outDataValue,
					sizeof(std::uint32_t));
				return true;
			}
			else
				return false;
		case ElementLayout::E5Z9Y9X9_UFloat:
			if (type == ElementType::Float)
			{
				std::uint32_t outDataValue =
					glm::packF3x9_E1x5(glm::vec3(m_values[0], m_values[1], m_values[2]));
				std::memcpy(reinterpret_cast<std::uint32_t*>(outData), &outDataValue,
					sizeof(std::uint32_t));
				return true;
			}
			else
				return false;
		default:
			return false;
	}
}

bool VertexValue::toData(void* outData, ElementLayout layout, ElementType type,
	const VertexValue& boundsMin, const VertexValue& boundsMax) const
{
	switch (type)
	{
		case ElementType::UNorm:
		{
			VertexValue boundedValues;
			for (unsigned int i = 0; i < count; ++i)
			{
				double range = boundsMax.m_values[i] - boundsMin.m_values[i];
				if (glm::epsilonEqual(range, 0.0, glm::epsilon<double>()))
					continue;

				boundedValues.m_values[i] = (m_values[i] - boundsMin.m_values[i])/range;
			}
			return boundedValues.toData(outData,layout, type);
		}
		case ElementType::SNorm:
		{
			VertexValue boundedValues;
			for (unsigned int i = 0; i < count; ++i)
			{
				double range = boundsMax.m_values[i] - boundsMin.m_values[i];
				if (glm::epsilonEqual(range, 0.0, glm::epsilon<double>()))
					continue;

				boundedValues.m_values[i] = (m_values[i] - boundsMin.m_values[i])/range;
				boundedValues.m_values[i] = boundedValues[i]*2.0 - 1.0;
			}
			return boundedValues.toData(outData,layout, type);
		}
		default:
			return toData(outData, layout, type);
	}
}

} // namespace vfc
