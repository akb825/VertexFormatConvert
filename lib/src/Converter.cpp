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
#include <VFC/VertexValue.h>
#include <cassert>
#include <iostream>
#include <limits>

namespace vfc
{

void Converter::stderrErrorFunction(const char* message)
{
	std::cerr << message << std::endl;
}

Converter::Converter(VertexFormat format, IndexType indexType, PrimitiveType primitiveType,
	unsigned int patchPoints, ErrorFunction errorFunction)
	: Converter(std::move(format), indexType, primitiveType, patchPoints,
		maxIndexValue(indexType), std::move(errorFunction))
{
}

Converter::Converter(VertexFormat format, IndexType indexType, PrimitiveType primitiveType,
	unsigned int patchPoints, std::uint32_t maxIndexValue, ErrorFunction errorFunction)
	: m_format(std::move(format)), m_indexType(indexType), m_primitiveType(primitiveType),
		m_patchPoints(patchPoints), m_maxIndexValue(maxIndexValue),
		m_errorFunction(std::move(errorFunction)),
		m_elementMapping(m_format.size(), VertexElementRef{0, nullptr}), m_vertexCount(0),
		m_indexCount(0)
{
	assert(!m_format.empty());
	assert(m_primitiveType != PrimitiveType::PatchList || m_patchPoints > 0);
	assert(indexType == IndexType::NoIndices ||
		m_maxIndexValue <= primitiveRestartIndexValue(indexType));
}

bool Converter::addVertexStream(VertexFormat vertexFormat, const void* vertexData,
	std::uint32_t vertexCount, IndexType indexType, const void* indexData,
	std::uint32_t indexCount)
{
	bool hasIndices = indexType != IndexType::NoIndices;
	if (!vertexData || (hasIndices && !indexData))
	{
		logError("Invalid vertex stream parameters.");
		return false;
	}

	std::uint32_t finalIndexCount = hasIndices ? indexCount : vertexCount;
	if (m_indexCount > 0 && m_indexCount != finalIndexCount)
	{
		if (hasIndices)
			logError("Mismatch between number of indices for vertex streams.");
		else
			logError("Mismatch between number of non-indexed vertices for vertex streams.");
		return false;
	}

	if (!isVertexCountValid(m_primitiveType, finalIndexCount, m_patchPoints))
	{
		if (hasIndices)
			logError("Invalid index count for requested primitive.");
		else
			logError("Invalid non-indexed vertex count for requested primitive.");
		return false;
	}

	bool hasElements = false;
	bool duplicateElements = false;
	for (std::size_t i = 0; i < m_format.size(); ++i)
	{
		if (vertexFormat.find(m_format[i].name) == vertexFormat.end())
			continue;

		hasElements = true;
		if (m_elementMapping[i].element)
		{
			std::string message = "Vertex element '";
			message += m_format[i].name;
			message += "' is present in multiple vertex streams.";
			logError(message.c_str());
			duplicateElements = true;
		}
	}

	m_indexCount = finalIndexCount;
	if (duplicateElements)
		return false;
	else if (!hasElements)
		return true;

	auto streamIndex = static_cast<std::uint32_t>(m_vertexStreams.size());
	for (std::size_t i = 0; i < m_format.size(); ++i)
	{
		if (vertexFormat.find(m_format[i].name) == vertexFormat.end())
			continue;

		m_elementMapping[i].streamIndex = streamIndex;
		m_elementMapping[i].element = &vertexFormat[i];
	}

	m_vertexStreams.push_back(
		VertexStream{vertexData, indexData, std::move(vertexFormat), vertexCount, indexType});
	return true;
}

void Converter::logError(const char* message) const
{
	if (m_errorFunction)
		m_errorFunction(message);
}

} // namespace vfc
