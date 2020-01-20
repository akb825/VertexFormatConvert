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
	 * @param format The vertex format to convert to.
	 * @param indexType The index type to convert to.
	 * @param primitiveType The primitive type for the geometry. This is used to guarantee
	 *     correctness if multiple index buffers are created due to exceeding the maximum index
	 *     value.
	 * @param patchPoints The number of points when primitiveType is PrimitiveType::PatchList.
	 * @param errorFunction Function to report error messages.
	 */
	Converter(VertexFormat format, IndexType indexType, PrimitiveType primitiveType,
		unsigned int patchPoints = 0, ErrorFunction errorFunction = &stderrErrorFunction);

	/**
	 * @brief Constructs the converter with information for what to convert to.
	 * @param format The vertex format to convert to.
	 * @param indexType The index type to convert to.
	 * @param primitiveType The primitive type for the geometry. This is used to guarantee
	 *     correctness if multiple index buffers are created due to exceeding the maximum index
	 *     value.
	 * @param patchPoints The number of points when primitiveType is PrimitiveType::PatchList.
	 * @param maxIndexValue The maximum index value to use.
	 * @param errorFunction Function to report error messages.
	 */
	Converter(VertexFormat format, IndexType indexType, PrimitiveType primitiveType,
		unsigned int patchPoints, std::uint32_t maxIndexValue,
		ErrorFunction errorFunction = &stderrErrorFunction);

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

private:
	void logError(const char* message) const;

	struct VertexStream
	{
		const void* vertexData;
		const void* indexData;
		VertexFormat vertexFormat;
		std::uint32_t vertexCount;
		IndexType indexType;
	};

	struct VertexElementRef
	{
		std::uint32_t streamIndex;
		const VertexElement* element;
	};

	VertexFormat m_format;
	IndexType m_indexType;
	PrimitiveType m_primitiveType;
	unsigned int m_patchPoints;
	std::uint32_t m_maxIndexValue;
	ErrorFunction m_errorFunction;

	std::vector<VertexStream> m_vertexStreams;
	std::vector<VertexElementRef> m_elementMapping;
	std::vector<std::uint8_t> m_vertives;
	std::vector<std::uint8_t> m_indices;
	std::vector<IndexData> m_indexData;
	std::uint32_t m_vertexCount;
	std::uint32_t m_indexCount;
};

} // namespace vfc
