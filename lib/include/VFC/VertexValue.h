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
#include <VFC/Export.h>
#include <algorithm>
#include <cassert>
#include <cstdint>

namespace vfc
{

enum class ElementLayout : std::int8_t;
enum class ElementType : std::int8_t;

/**
 * @brief Class that contains the value of a vertex, which can be converted between formats.
 *
 * This contains the value represented as four doubles as an intermediate storage value. The default
 * value is (0, 0, 0, 1), and any components not present during decoding will use the default value.
 */
class VFC_EXPORT VertexValue
{
public:
	/**
	 * @brief The number of elements.
	 */
	static constexpr unsigned int count = 4;

	/**
	 * @brief The initial minimum value for bounds.
	 */
	static const VertexValue initialBoundsMin;

	/**
	 * @brief The initial maximum value for bounds.
	 */
	static const VertexValue initialBoundsMax;

	/**
	 * @brief Constructs the value.
	 * @param x The first value.
	 * @param y The second value.
	 * @param z The third value.
	 * @param w The fourth value.
	 */
	explicit VertexValue(double x = 0, double y = 0, double z = 0, double w = 1)
	{
		m_values[0] = x;
		m_values[1] = y;
		m_values[2] = z;
		m_values[3] = w;
	}

	/**
	 * @brief Accesses a value.
	 * @param i The index.
	 * @return The value.
	 */
	double& operator[](unsigned int i)
	{
		assert(i < count);
		return m_values[i];
	}

	/** @copydoc operator[]() */
	double operator[](unsigned int i) const
	{
		assert(i < count);
		return m_values[i];
	}

	/**
	 * @brief Compares two values.
	 * @param other The other value.
	 * @return True if the values are equal.
	 */
	bool operator==(const VertexValue& other) const
	{
		return m_values[0] == other.m_values[0] && m_values[1] == other.m_values[1] &&
			m_values[2] == other.m_values[2] && m_values[3] == other.m_values[3];
	}

	/**
	 * @brief Compares two values.
	 * @param other The other value.
	 * @return True if the values are not equal.
	 */
	bool operator!=(const VertexValue& other) const
	{
		return !(*this == other);
	}

	/**
	 * @brief Expands bounds based on the current value.
	 * @param[inout] minVal The minimum value of the bounds. This should be initialized to
	 *     initialBoundsMin.
	 * @param[inout] maxVal The maximum value of the bounds. This should be initialized to
	 *     initialBoundsMax.
	 */
	void expandBounds(VertexValue& minVal, VertexValue& maxVal) const
	{
		for (unsigned int i = 0; i < count; ++i)
		{
			minVal.m_values[i] = std::min(m_values[i], minVal.m_values[i]);
			maxVal.m_values[i] = std::max(m_values[i], maxVal.m_values[i]);
		}
	}

	/**
	 * @brief Reads the value from vertex data.
	 * @param data The data to populate with.
	 * @param layout The element layout.
	 * @param type The element type.
	 * @return False if the parameters are invalid.
	 */
	bool fromData(const void* data, ElementLayout layout, ElementType type);

	/**
	 * @brief Writes the value to vertex data.
	 * @param[out] outData The data to populate.
	 * @param layout The element layout.
	 * @param type The element type.
	 * @return False if the parameters are invalid.
	 */
	bool toData(void* outData, ElementLayout layout, ElementType type) const;

	/**
	 * @brief Writes the value to vertex data.
	 *
	 * This will normalize the values into the bounds min and max values before packing into the
	 * data. When type is ElementType::UNorm it will be normalized in the range [0, 1], and when
	 * type is ElementType::SNorm it will be normalized in the range [-1, 1]. Otherwise the bounds
	 * are ignored.
	 *
	 * @param[out] outData The data to populate.
	 * @param layout The element layout.
	 * @param type The element type.
	 * @param boundsMin The minimum value of the bounds.
	 * @param boundsMax The maximum value of the bounds.
	 * @return False if the parameters are invalid.
	 */
	bool toData(void* outData, ElementLayout layout, ElementType type, const VertexValue& boundsMin,
		const VertexValue& boundsMax) const;

private:
	double m_values[count];
};

} // namespace vfc
