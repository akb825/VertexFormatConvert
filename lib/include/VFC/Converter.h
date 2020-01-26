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
#include <VFC/IndexData.h>
#include <VFC/VertexFormat.h>
#include <VFC/VertexValue.h>
#include <cstdint>
#include <functional>
#include <vector>

namespace vfc
{

/**
 * @brief Converter for vertex and index data.
 *
 * This class takes multiple vertex streams as index, optionally with index values, and converts
 * them into a single vertex stream.
 */
class VFC_EXPORT Converter
{
public:
	/**
	 * @brief Enum for the transform to apply to a vertex value.
	 */
	enum class Transform
	{
		Identity,     ///< Use the value is used as-is.
		/**
		 * Normalizes the values to the bounds. This will take the bounding box for all of the
		 * vertex values for the element, then the values are converted to a value in the range
		 * [0, 1] for UNorm types or [-1, 1] for SNorm types. When used with non-normalized types,
		 * this will be the same as Identity.
		 */
		Bounds,
		UNormToSNorm, ///< Converts a value in the range [0, 1] to the range [-1, 1].
		SNormToUNorm  ///< Converts a value in the range [-1, 1] to the range [0, 1].
	};

	/**
	 * @brief Type for a function to handle errors.
	 * @param message The message to log.
	 */
	using ErrorFunction = std::function<void(const char* message)>;

	/**
	 * @brief Error function that prints the message to stderr.
	 * @param message The message to log.
	 */
	static void stderrErrorFunction(const char* message);

	/**
	 * @brief Constructs the converter with information for what to convert to.
	 *
	 * This uses the default maximum index value.
	 *
	 * @param vertexFormat The vertex format to convert to.
	 * @param indexType The index type to convert to.
	 * @param primitiveType The primitive type for the geometry. This is used to guarantee
	 *     correctness if multiple index buffers are created due to exceeding the maximum index
	 *     value.
	 * @param patchPoints The number of points when primitiveType is PrimitiveType::PatchList.
	 * @param errorFunction Function to report error messages.
	 */
	Converter(VertexFormat vertexFormat, IndexType indexType, PrimitiveType primitiveType,
		unsigned int patchPoints = 0, ErrorFunction errorFunction = &stderrErrorFunction);

	/**
	 * @brief Constructs the converter with information for what to convert to.
	 * @param vertexFormat The vertex format to convert to.
	 * @param indexType The index type to convert to.
	 * @param primitiveType The primitive type for the geometry. This is used to guarantee
	 *     correctness if multiple index buffers are created due to exceeding the maximum index
	 *     value.
	 * @param patchPoints The number of points when primitiveType is PrimitiveType::PatchList.
	 * @param maxIndexValue The maximum index value to use.
	 * @param errorFunction Function to report error messages.
	 */
	Converter(VertexFormat vertexFormat, IndexType indexType, PrimitiveType primitiveType,
		unsigned int patchPoints, std::uint32_t maxIndexValue,
		ErrorFunction errorFunction = &stderrErrorFunction);

	Converter(const Converter& other) = delete;
	Converter& operator=(const Converter& other) = delete;

	/**
	 * @brief Move constructor.
	 * @param other The other instance to move.
	 */
	Converter(Converter&& other) noexcept = default;

	/**
	 * @brief Move assignment.
	 * @param other The other instance to move.
	 * @return A reference to this.
	 */
	Converter& operator=(Converter&& other) noexcept = default;

	/**
	 * @brief Gets the vertex format to convert to.
	 * @return The vertex format.
	 */
	const VertexFormat& getVertexFormat() const
	{
		return m_vertexFormat;
	}

	/**
	 * @brief Gets the index type to convert to.
	 * @return The index type.
	 */
	IndexType getIndexType() const
	{
		return m_indexType;
	}

	/**
	 * @brief Gets the primitive type for the geometry.
	 * @return The primitive type.
	 */
	PrimitiveType getPrimitiveType() const
	{
		return m_primitiveType;
	}

	/**
	 * @brief Gets the number of patch points.
	 * @return The patch points.
	 */
	unsigned int getPatchPoints() const
	{
		return m_patchPoints;
	}

	/**
	 * @brief Gets the max allowed index value.
	 * @return The max index value.
	 */
	std::uint32_t getMaxIndexValue() const
	{
		return m_maxIndexValue;
	}

	/**
	 * @brief Gets the transform for a vertex element by index.
	 * @param i The index for the element.
	 * @return The transform.
	 */
	Transform getElementTransform(std::size_t i) const
	{
		assert(i < m_elementMapping.size());
		return m_elementMapping[i].transform;
	}

	/**
	 * @brief Gets the transform for a vertex element by name.
	 * @param name The name of the element.
	 * @return The transform.
	 */
	Transform getElementTransform(const char* name) const;

	/**
	 * @brief Gets the transform for a vertex element by name.
	 * @param name The name of the element.
	 * @return The transform.
	 */
	Transform getElementTransform(const std::string& name) const
	{
		return getElementTransform(name.c_str());
	}

	/**
	 * @brief Sets the transform for a vertex element by index.
	 * @param i The index of the element.
	 * @param transform The transform to set.
	 */
	void setElementTransform(std::size_t i, Transform transform)
	{
		assert(i < m_elementMapping.size());
		m_elementMapping[i].transform = transform;
	}

	/**
	 * @brief Sets the transform for a vertex element by name.
	 * @param name The name of the element.
	 * @param transform The transform to set.
	 * @return False if the element wasn't found.
	 */
	bool setElementTransform(const char* name, Transform transform);

	/**
	 * @brief Sets the transform for a vertex element by name.
	 * @param name The name of the element.
	 * @param transform The transform to set.
	 * @return False if the element wasn't found.
	 */
	bool setElementTransform(const std::string& name, Transform transform)
	{
		return setElementTransform(name.c_str(), transform);
	}

	/**
	 * @brief Adds a vertex stream to convert without indices.
	 * @param vertexFormat The vertex format.
	 * @param vertexData The vertex data.
	 * @param vertexCount The number of vertices.
	 * @return False if the vertex stream couldn't be added.
	 */
	bool addVertexStream(VertexFormat vertexFormat, const void* vertexData,
		std::uint32_t vertexCount)
	{
		return addVertexStream(std::move(vertexFormat), vertexData, vertexCount,
			IndexType::NoIndices, nullptr, 0);
	}

	/**
	 * @brief Adds a vertex stream to convert with indices.
	 * @param vertexFormat The vertex format.
	 * @param vertexData The vertex data.
	 * @param vertexCount The number of vertices.
	 * @param indexType The type of the index data.
	 * @param indexData The index data.
	 * @param indexCount The number of indices.
	 * @return False if the vertex stream couldn't be added.
	 */
	bool addVertexStream(VertexFormat vertexFormat, const void* vertexData,
		std::uint32_t vertexCount, IndexType indexType, const void* indexData,
		std::uint32_t indexCount);

	/**
	 * @brief Performs the conversion from the input streams to the converted vertex and index data.
	 * @return False if an error occurred.
	 */
	bool convert();

	/**
	 * @brief Gets the converted indices.
	 * @return The index data. More than one buffer will be returned if multiple base vertex values
	 *     are required.
	 */
	const std::vector<IndexData>& getIndices() const
	{
		return m_indexData;
	}

	/**
	 * @brief Gets the bounds for a vertex element.
	 *
	 * The bounds are based on the input values before any transforms are applied.
	 *
	 * @param[out] outMin The minimum value for the bounds.
	 * @param[out] outMax The maximum value for the bounds.
	 * @param i The index of the vertex element.
	 */
	void getVertexElementBounds(VertexValue& outMin, VertexValue& outMax, std::size_t i) const
	{
		assert(i < m_elementMapping.size());
		outMin = m_elementMapping[i].minVal;
		outMax = m_elementMapping[i].maxVal;
	}

	/**
	 * @brief Gets the bounds for a vertex element.
	 *
	 * The bounds are based on the input values before any transforms are applied.
	 *
	 * @param[out] outMin The minimum value for the bounds.
	 * @param[out] outMax The maximum value for the bounds.
	 * @param name The name of the vertex element.
	 * @return False if the element wasn't found.
	 */
	bool getVertexElementBounds(VertexValue& outMin, VertexValue& outMax, const char* name) const;

	/**
	 * @brief Gets the bounds for a vertex element.
	 *
	 * The bounds are based on the input values before any transforms are applied.
	 *
	 * @param[out] outMin The minimum value for the bounds.
	 * @param[out] outMax The maximum value for the bounds.
	 * @param name The name of the vertex element.
	 * @return False if the element wasn't found.
	 */
	bool getVertexElementBounds(VertexValue& outMin, VertexValue& outMax,
		const std::string& name) const
	{
		return getVertexElementBounds(outMin, outMax, name.c_str());
	}

	/**
	 * @brief Gets the converted vertices.
	 * @return The vertices as an array of bytes.
	 */
	const std::vector<std::uint8_t>& getVertices() const
	{
		return m_vertices;
	}

	/**
	 * @brief Gets the number of converted vertices.
	 * @return The number of vertices.
	 */
	std::uint32_t getVertexCount() const
	{
		return static_cast<std::uint32_t>(m_vertices.size()/m_vertexFormat.stride());
	}

private:
	void logError(const char* message) const;

	struct VertexStream
	{
		const std::uint8_t* vertexData;
		const void* indexData;
		VertexFormat vertexFormat;
		std::uint32_t vertexCount;
		IndexType indexType;
	};

	struct VertexElementRef
	{
		std::uint32_t streamIndex;
		const VertexElement* element;
		Transform transform;
		VertexValue minVal;
		VertexValue maxVal;
	};

	VertexFormat m_vertexFormat;
	IndexType m_indexType;
	PrimitiveType m_primitiveType;
	unsigned int m_patchPoints;
	std::uint32_t m_maxIndexValue;
	ErrorFunction m_errorFunction;

	std::vector<VertexStream> m_vertexStreams;
	std::vector<VertexElementRef> m_elementMapping;
	std::vector<std::uint8_t> m_vertices;
	std::vector<std::uint8_t> m_indices;
	std::vector<IndexData> m_indexData;
	std::uint32_t m_indexCount;
};

} // namespace vfc
