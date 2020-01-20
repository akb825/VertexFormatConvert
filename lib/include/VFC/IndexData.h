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

#pragma once

#include <VFC/Config.h>
#include <cstdint>
#include <limits>

/**
 * @file
 * @brief Functions and types for working with index data.
 */

namespace vfc
{

/**
 * @brief Enum for the type of an index.
 */
enum class IndexType : std::uint8_t
{
	NoIndices, ///< No index data is provided.
	UInt16,    ///< uint16_t
	UInt32     ///< uint32_t
};

/**
 * @brief Struct containing the information for index data.
 */
struct IndexData
{
	/**
	 * @brief The data for the indices.
	 */
	void* data;

	/**
	 * @brief The type of the index data.
	 */
	IndexType type;

	/**
	 * @brief The number of indices.
	 */
	std::uint32_t count;

	/**
	 * @brief The base vertex.
	 *
	 * This should be added to the index value to get the "real" index. When drawing the data, this
	 * can usually be provided to the draw function. If the graphics API used doesn't support base
	 * vertices directly, it can be applied as a byte offset when binding the vertex buffer.
	 */
	std::int32_t baseVertex;
};

/**
 * @brief Gets the value of an index.
 * @param type The type of the index data.
 * @param data The index data.
 * @param i The index to access.
 * @return The index value, or std::numeric_limits<std::uint32_t>::max() if parameters are invalid.
 */
inline std::uint32_t getIndexValue(IndexType type, const void* data, std::uint32_t i)
{
	if (!data)
		return std::numeric_limits<std::uint32_t>::max();

	switch (type)
	{
		case IndexType::UInt16:
			return reinterpret_cast<const std::uint16_t*>(data)[i];
		case IndexType::UInt32:
			return reinterpret_cast<const std::uint32_t*>(data)[i];
		default:
			return std::numeric_limits<std::uint32_t>::max();
	}
}

/**
 * @brief Sets the value of an index.
 * @param type The type of the index data.
 * @param data The index data.
 * @param i The index to access.
 * @param value The index value to set. This may be truncated.
 * @return False if the type or data are invalid.
 */
inline bool setIndexValue(IndexType type, void* data, std::uint32_t i, std::uint32_t value)
{
	if (!data)
		return false;

	switch (type)
	{
		case IndexType::UInt16:
			reinterpret_cast<std::uint16_t*>(data)[i] = static_cast<std::uint16_t>(value);
			return true;
		case IndexType::UInt32:
			reinterpret_cast<std::uint32_t*>(data)[i] = value;
			return true;
		default:
			return false;
	}
}

/**
 * @brief Gets the max value of an index.
 *
 * This is one less than the max value of the integer type since the max value is typically used as
 * a sentinal value for primitive resarting.
 */
inline constexpr std::uint32_t maxIndexValue(IndexType type)
{
	// Need to have a single return statement to satisfy constexpr.
	return type == IndexType::UInt16 ? std::numeric_limits<std::uint16_t>::max() - 1 :
		(type == IndexType::UInt32 ? std::numeric_limits<std::uint32_t>::max() - 1 : 0);
}

/**
 * @brief Gets the primitive restart value of an index.
 */
inline constexpr std::uint32_t primitiveRestartIndexValue(IndexType type)
{
	// Need to have a single return statement to satisfy constexpr.
	return type == IndexType::UInt16 ? std::numeric_limits<std::uint16_t>::max() :
		(type == IndexType::UInt32 ? std::numeric_limits<std::uint32_t>::max() : 0);
}

} // namespace vfc

