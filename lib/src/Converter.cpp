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
#include <VFC/VertexValue.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <unordered_set>

namespace vfc
{

namespace
{

constexpr std::uint32_t hashSeed = 0xc70f6907U;

// MurmurHash2, with some adjustments for code clarity and alignment guarantees.
// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp
#if VFC_64BIT
std::size_t murmurHash2(const std::uint8_t* data, std::uint32_t size)
{
	const std::uint64_t m = 0xc6a4a7935bd1e995ULL;
	const int r = 47;

	std::uint64_t h = hashSeed ^ (size * m);

	while (size >= 8)
	{
		std::uint64_t k;
		std::memcpy(&k, data, sizeof(std::uint64_t));

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;

		data += 8;
		size -= 8;
	}

	switch (size)
	{
		case 7: h ^= std::uint64_t(data[6]) << 48;
		case 6: h ^= std::uint64_t(data[5]) << 40;
		case 5: h ^= std::uint64_t(data[4]) << 32;
		case 4: h ^= std::uint64_t(data[3]) << 24;
		case 3: h ^= std::uint64_t(data[2]) << 16;
		case 2: h ^= std::uint64_t(data[1]) << 8;
		case 1: h ^= std::uint64_t(data[0]);
		h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}
#else
inline void mmix(std::uint32_t& h, std::uint32_t k, std::uint32_t m)
{
	const int r = 24;
	k *= m;
	k ^= k >> r;
	k *= m;
	h *= m;
	h ^= k;
}

std::size_t murmurHash2(const std::uint8_t* data, std::uint32_t size)
{
	const std::uint32_t m = 0x5bd1e995;
	std::uint32_t l = size;
	std::uint32_t h = hashSeed;

	while (size >= 4)
	{
		std::uint32_t k;
		std::memcpy(&k, data, sizeof(std::uint32_t));

		mmix(h, k, m);

		data += 4;
		size -= 4;
	}

	std::uint32_t t = 0;
	switch (size)
	{
		case 3: t ^= data[2] << 16;
		case 2: t ^= data[1] << 8;
		case 1: t ^= data[0];
	}

	mmix(h, t, m);
	mmix(h, l, m);

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}
#endif

std::size_t hashCombine(std::size_t seed, std::size_t value)
{
	return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

struct VertexRef
{
	VertexRef(const std::vector<std::vector<std::uint8_t>>& _data,
		const std::vector<VertexFormat>& _format, std::uint32_t _index)
		: data(&_data), format(&_format), index(_index)
	{
		assert(data->size() == format->size());
	}

	const std::vector<std::vector<std::uint8_t>>* data;
	const std::vector<VertexFormat>* format;
	std::uint32_t index;

	bool operator==(const VertexRef& other) const
	{
		assert(data == other.data);
		assert(format == other.format);
		for (std::size_t i = 0; i < data->size(); ++i)
		{
			std::size_t stride = (*format)[i].stride();
			std::size_t thisOffset = index*stride;
			std::size_t otherOffset = other.index*stride;
			if (std::memcmp((*data)[i].data() + thisOffset,
					(*other.data)[i].data() + otherOffset, stride) != 0)
			{
				return false;
			}
		}

		return true;
	}

	bool operator!=(const VertexRef& other) const
	{
		return !(*this == other);
	}
};

struct VertexHash
{
	std::size_t operator()(const VertexRef& vertex) const
	{
		std::size_t hash = 0;
		for (std::size_t i = 0; i < vertex.data->size(); ++i)
		{
			std::uint32_t stride = (*vertex.format)[i].stride();
			std::size_t offset = vertex.index*size_t(stride);
			hash = hashCombine(hash, murmurHash2((*vertex.data)[i].data() + offset, stride));
		}

		return hash;
	}
};

using VertexSet = std::unordered_set<VertexRef, VertexHash>;

inline std::vector<VertexFormat> singleVertexFormat(VertexFormat&& baseFormat)
{
	std::vector<VertexFormat> formatVec;
	formatVec.push_back(std::move(baseFormat));
	return formatVec;
}

std::uint32_t addVertex(std::vector<std::vector<std::uint8_t>>& vertices,
	const std::vector<VertexFormat>& vertexFormat,
	const std::vector<std::vector<std::uint8_t>>& newVertex, VertexSet& vertexSet)
{
	assert(!vertices.empty());
#if VFC_DEBUG
	for (std::size_t i = 0; i < vertices.size(); ++i)
		assert(vertices[i].size() % vertexFormat[i].stride() == 0);
#endif

	// Expected that almost always adding a new vertex, so optimize for that.
	auto index = static_cast<std::uint32_t>(vertices[0].size()/vertexFormat[0].stride());
	for (std::size_t i = 0; i < vertices.size(); ++i)
		vertices[i].insert(vertices[i].end(), newVertex[i].begin(), newVertex[i].end());
	VertexRef ref(vertices, vertexFormat, index);
	auto insertPair = vertexSet.insert(ref);
	if (!insertPair.second)
	{
		assert(insertPair.first->index < ref.index);
		// Remove the added vertex data.
		for (std::size_t i = 0; i < vertices.size(); ++i)
			vertices[i].resize(vertices[i].size() - vertexFormat[i].stride());
		return insertPair.first->index;
	}

	return index;
}

std::uint32_t addVertex(std::vector<std::vector<std::uint8_t>>& vertices,
	const std::vector<VertexFormat>& vertexFormat, std::uint32_t index, VertexSet& vertexSet)
{
	assert(!vertices.empty());
#if VFC_DEBUG
	for (std::size_t i = 0; i < vertices.size(); ++i)
		assert(vertices[i].size() % vertexFormat[i].stride() == 0);
#endif

	// Expected that almost always adding a new vertex, so optimize for that.
	auto newIndex = static_cast<std::uint32_t>(vertices[0].size()/vertexFormat[0].stride());
	for (std::size_t i = 0; i < vertices.size(); ++i)
	{
		std::size_t stride = vertexFormat[i].stride();
		std::size_t offset = index*stride;
		vertices[i].insert(vertices[i].end(), vertices[i].begin() + offset,
			vertices[i].begin() + offset + stride);
	}
	VertexRef ref(vertices, vertexFormat, newIndex);
	auto insertPair = vertexSet.insert(ref);
	if (!insertPair.second)
	{
		assert(insertPair.first->index < ref.index);
		// Remove the added vertex data.
		for (std::size_t i = 0; i < vertices.size(); ++i)
			vertices.resize(vertices[i].size() - vertexFormat[i].stride());
		return insertPair.first->index;
	}

	return newIndex;
}

void addIndex(std::vector<std::uint8_t>& indices, IndexType type, unsigned int sizeofIndex,
	std::uint32_t value)
{
	std::size_t nextIndex = indices.size()/sizeofIndex;
	indices.resize(indices.size() + sizeofIndex);
	setIndexValue(type, indices.data(), nextIndex, value);
}

bool isPrimitiveRestart(std::uint32_t index, std::uint32_t primitiveRestart,
	PrimitiveType primitiveType)
{
	switch (primitiveType)
	{
		case PrimitiveType::LineStrip:
		case PrimitiveType::TriangleStrip:
		case PrimitiveType::TriangleFan:
			return index == primitiveRestart;
		default:
			return false;
	}
}

unsigned int primitiveMinIndexCount(PrimitiveType type, unsigned int patchPoints)
{
	switch (type)
	{
		case PrimitiveType::PointList:
			return 1;
		case PrimitiveType::LineList:
		case PrimitiveType::LineStrip:
			return 2;
		case PrimitiveType::TriangleList:
		case PrimitiveType::TriangleStrip:
		case PrimitiveType::TriangleFan:
			return 3;
		case PrimitiveType::PatchList:
			return std::max(patchPoints, 1U);
		default:
			return 1;
	}
}

unsigned int primitiveIndexStride(PrimitiveType type, unsigned int patchPoints)
{
	switch (type)
	{
		case PrimitiveType::Invalid:
			assert(false);
			return 0;
		case PrimitiveType::PointList:
		case PrimitiveType::LineStrip:
		case PrimitiveType::TriangleStrip:
		case PrimitiveType::TriangleFan:
			return 1;
		case PrimitiveType::LineList:
			return 2;
		case PrimitiveType::TriangleList:
			return 3;
		case PrimitiveType::PatchList:
			return patchPoints;
	}

	assert(false);
	return 0;
}

void copyVertex(std::vector<std::vector<std::uint8_t>>& vertices,
	const std::vector<VertexFormat>& vertexFormat, VertexSet& vertexSet,
	std::vector<std::uint8_t>& indices, IndexType indexType, unsigned int sizeofIndex,
	std::uint32_t baseVertex, std::uint32_t indexIndex, std::int32_t prevBaseVertex)
{
	std::uint32_t prevIndex = getIndexValue(indexType, indices.data(), indexIndex) + prevBaseVertex;
	std::uint32_t newIndex = addVertex(vertices, vertexFormat, prevIndex, vertexSet);
	addIndex(indices, indexType, sizeofIndex, newIndex - baseVertex);
}

void copyConnectedVertices(std::vector<std::vector<std::uint8_t>>& vertices,
	const std::vector<VertexFormat>& vertexFormat, VertexSet& vertexSet,
	std::vector<std::uint8_t>& indices, IndexType indexType, unsigned int sizeofIndex,
	std::int32_t baseVertex, PrimitiveType primitiveType, std::uint32_t lastRestartIndex,
	std::uint32_t& prevIndexCount, std::int32_t prevBaseVertex, std::uint32_t& curIndexCount)
{
#if VFC_DEBUG
	for (std::size_t i = 0; i < vertices.size(); ++i)
		assert(static_cast<std::size_t>(baseVertex) == vertices[i].size()/vertexFormat[i].stride());
#endif

	auto indexCount = static_cast<std::uint32_t>(indices.size()/sizeofIndex);
	switch (primitiveType)
	{
		case PrimitiveType::LineStrip:
			if (lastRestartIndex != indexCount - 1)
			{
				copyVertex(vertices, vertexFormat, vertexSet, indices, indexType, sizeofIndex,
					baseVertex, indexCount - 1, prevBaseVertex);
				++curIndexCount;
			}
			break;
		case PrimitiveType::TriangleStrip:
		{
			std::uint32_t firstIndex = lastRestartIndex + 1;
			std::uint32_t stripIndexCount = indexCount - firstIndex + 1;
			if (stripIndexCount <= 2)
			{
				for (std::uint32_t k = firstIndex; k < indexCount; ++k)
				{
					copyVertex(vertices, vertexFormat, vertexSet, indices, indexType, sizeofIndex,
						baseVertex, k, prevBaseVertex);
					++curIndexCount;
				}
			}
			else
			{
				// Reverse every other primitive.
				std::uint32_t primitiveCount = stripIndexCount - 2;
				if (primitiveCount & 1)
				{
					copyVertex(vertices, vertexFormat, vertexSet, indices, indexType, sizeofIndex,
						baseVertex, indexCount - 1, prevBaseVertex);
					copyVertex(vertices, vertexFormat, vertexSet, indices, indexType, sizeofIndex,
						baseVertex, indexCount - 2, prevBaseVertex);
				}
				else
				{
					copyVertex(vertices, vertexFormat, vertexSet, indices, indexType, sizeofIndex,
						baseVertex, indexCount - 2, prevBaseVertex);
					copyVertex(vertices, vertexFormat, vertexSet, indices, indexType, sizeofIndex,
						baseVertex, indexCount - 1, prevBaseVertex);
				}
				curIndexCount += 2;
			}
			break;
		}
		case PrimitiveType::TriangleFan:
			if (lastRestartIndex != indexCount - 1)
			{
				// First vertex in the fan.
				copyVertex(vertices, vertexFormat, vertexSet, indices, indexType, sizeofIndex,
					baseVertex, lastRestartIndex + 1, prevBaseVertex);
				++curIndexCount;
				if (lastRestartIndex != indexCount - 2)
				{
					// Last point to continue for the triangle.
					copyVertex(vertices, vertexFormat, vertexSet, indices, indexType, sizeofIndex,
						baseVertex, indexCount - 1, prevBaseVertex);
					++curIndexCount;
				}
			}
			break;
		default:
			break;
	}

	// If we had to copy all of the vertices since the last restart, there wasn't a full primitive
	// and the number of indices for the last index buffer must be reduced.
	auto addedIndices = static_cast<std::uint32_t>(indices.size()/sizeofIndex) - indexCount;
	if (lastRestartIndex + 1 == indexCount - addedIndices)
		prevIndexCount -= addedIndices;
}

} // namespace

void Converter::stderrErrorFunction(const char* message)
{
	std::cerr << message << std::endl;
}

Converter::Converter(VertexFormat vertexFormat, IndexType indexType, PrimitiveType primitiveType,
	unsigned int patchPoints, ErrorFunction errorFunction)
	: Converter(singleVertexFormat(std::move(vertexFormat)), indexType, primitiveType, patchPoints,
		maxIndexValue(indexType), std::move(errorFunction))
{
}

Converter::Converter(VertexFormat vertexFormat, IndexType indexType, PrimitiveType primitiveType,
	unsigned int patchPoints, std::uint32_t maxIndexValue, ErrorFunction errorFunction)
	: Converter(singleVertexFormat(std::move(vertexFormat)), indexType, primitiveType, patchPoints,
		maxIndexValue, std::move(errorFunction))
{
}

Converter::Converter(std::vector<VertexFormat> vertexFormat, IndexType indexType,
	PrimitiveType primitiveType, unsigned int patchPoints, ErrorFunction errorFunction)
	: Converter(std::move(vertexFormat), indexType, primitiveType, patchPoints,
		maxIndexValue(indexType), std::move(errorFunction))
{
}

Converter::Converter(std::vector<VertexFormat> vertexFormat, IndexType indexType,
	PrimitiveType primitiveType, unsigned int patchPoints, std::uint32_t maxIndexValue,
	ErrorFunction errorFunction)
	: m_vertexFormat(std::move(vertexFormat))
	, m_indexType(indexType)
	, m_primitiveType(primitiveType)
	, m_patchPoints(patchPoints)
	, m_maxIndexValue(maxIndexValue)
	, m_errorFunction(std::move(errorFunction))
	, m_indexCount(0)
{
	bool error = false;
	if (m_vertexFormat.empty())
	{
		logError("Converter vertex format is empty.");
		error = true;
	}
	for (const VertexFormat& streamFormat : m_vertexFormat)
	{
		if (streamFormat.empty())
		{
			logError("Converter vertex format is empty.");
			error = true;
		}
	}

	if (m_primitiveType == PrimitiveType::Invalid)
	{
		logError("Converter primitive type is invalid.");
		error = true;
	}

	if (m_primitiveType == PrimitiveType::PatchList && m_patchPoints == 0)
	{
		logError(
			"Patch point count must be provided to Converter when using PatchList primitives.");
		error = true;
	}

	if (indexType != IndexType::NoIndices)
	{
		if (m_maxIndexValue < primitiveMinIndexCount(primitiveType, patchPoints) - 1)
		{
			logError("Max index value is too small to hold any primitives.");
			error = true;
		}
		else if (m_maxIndexValue > primitiveRestartIndexValue(indexType))
		{
			logError("Max index value is higher than the maximum for the type.");
			error = true;
		}
	}

	if (error)
		m_vertexFormat.clear();
	else
	{
		m_elementMapping.reserve(m_vertexFormat.size());
		for (std::size_t i = 0; i < m_vertexFormat.size(); ++i)
		{
			m_elementMapping.emplace_back(m_vertexFormat[i].size(), VertexElementRef{0, nullptr,
				Transform::Identity, VertexValue::initialBoundsMin, VertexValue::initialBoundsMax});
		}
	}
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
	std::string message;
	for (std::size_t i = 0; i < m_vertexFormat.size(); ++i)
	{
		const VertexFormat& curFormat = m_vertexFormat[i];
		const std::vector<VertexElementRef>& curElementMapping = m_elementMapping[i];
		for (std::size_t j = 0; j < curFormat.size(); ++j)
		{
			if (vertexFormat.find(curFormat[j].name) == vertexFormat.end())
				continue;

			hasElements = true;
			if (curElementMapping[j].element)
			{
				message = "Vertex element '";
				message += curFormat[j].name;
				message += "' is present in multiple vertex streams.";
				logError(message.c_str());
				duplicateElements = true;
			}
		}
	}

	m_indexCount = finalIndexCount;
	if (duplicateElements)
		return false;
	else if (!hasElements)
		return true;

	auto streamIndex = static_cast<std::uint32_t>(m_vertexStreams.size());
	for (std::size_t i = 0; i < m_vertexFormat.size(); ++i)
	{
		const VertexFormat& curFormat = m_vertexFormat[i];
		std::vector<VertexElementRef>& curElementMapping = m_elementMapping[i];
		for (std::size_t j = 0; j < curFormat.size(); ++j)
		{
			auto it = vertexFormat.find(curFormat[j].name);
			if (it == vertexFormat.end())
				continue;

			curElementMapping[j].streamIndex = streamIndex;
			curElementMapping[j].element = &*it;
		}
	}

	m_vertexStreams.push_back(VertexStream{reinterpret_cast<const std::uint8_t*>(vertexData),
		indexData, std::move(vertexFormat), vertexCount, indexType});
	return true;
}

Converter::Transform Converter::getElementTransform(const char* name) const
{
	for (std::size_t i = 0; i < m_vertexFormat.size(); ++i)
	{
		const VertexFormat& curFormat = m_vertexFormat[i];
		auto foundElement = curFormat.find(name);
		if (foundElement != curFormat.end())
			return m_elementMapping[i][foundElement - curFormat.begin()].transform;
	}

	return Transform::Identity;
}

bool Converter::setElementTransform(const char* name, Transform transform)
{
	for (std::size_t i = 0; i < m_vertexFormat.size(); ++i)
	{
		const VertexFormat& curFormat = m_vertexFormat[i];
		auto foundElement = curFormat.find(name);
		if (foundElement == curFormat.end())
			continue;

		m_elementMapping[i][foundElement - curFormat.begin()].transform = transform;
		return true;
	}

	return false;
}

void Converter::logError(const char* message) const
{
	if (m_errorFunction)
		m_errorFunction(message);
}

bool Converter::convert()
{
	if (!isValid())
	{
		logError("Converter is invalid.");
		return false;
	}

	if (!m_vertices.empty())
	{
		logError("Converter::convert() may only be called once.");
		return false;
	}

	bool hasAllElements = true;
	std::string message;
	for (std::size_t i = 0; i < m_vertexFormat.size(); ++i)
	{
		const VertexFormat& curFormat = m_vertexFormat[i];
		const std::vector<VertexElementRef>& curElementMapping = m_elementMapping[i];
		for (std::size_t j = 0; j < curFormat.size(); ++j)
		{
			if (curElementMapping[j].element)
				continue;

			message = "Vertex element '";
			message += curFormat[j].name;
			message += "' has no corresponding input vertex stream.";
			logError(message.c_str());
			hasAllElements = false;
		}
	}

	if (!hasAllElements)
		return false;

	// First need to gather the bounds. Loop over the streams first for better cache efficiency.
	for (std::vector<VertexElementRef>& curElementMapping : m_elementMapping)
	{
		for (VertexElementRef& elementRef : curElementMapping)
		{
			const VertexStream& stream = m_vertexStreams[elementRef.streamIndex];
			assert(elementRef.element);
			const VertexElement& element = *elementRef.element;
			std::uint32_t primitiveRestart = primitiveRestartIndexValue(stream.indexType);
			for (std::uint32_t i = 0; i < m_indexCount; ++i)
			{
				std::uint32_t indexValue = getIndexValue(stream.indexType, stream.indexData, i, i);
				if (isPrimitiveRestart(indexValue, primitiveRestart, m_primitiveType))
				{
					if (m_indexType == IndexType::NoIndices)
					{
						logError("Indices must be output if a primitive restart is used.");
						return false;
					}
					continue;
				}

				if (indexValue >= stream.vertexCount)
				{
					message = "Index value for vertex element '";
					message += element.name;
					message += "' is out of range.";
					logError(message.c_str());
					return false;
				}

				VertexValue value;
				auto offset = static_cast<std::size_t>(indexValue)*stream.vertexFormat.stride();
				value.fromData(stream.vertexData + offset, element.layout, element.type);
				value.expandBounds(elementRef.minVal, elementRef.maxVal);
			}
		}
	}

	// Create the combined vertex stream.
	std::vector<std::vector<std::uint8_t>> vertexData(m_vertexFormat.size());
	VertexSet vertexSet;
	for (std::size_t i = 0; i < m_vertexFormat.size(); ++i)
		vertexData[i].resize(m_vertexFormat[i].stride());
	m_vertices.resize(m_vertexFormat.size());

	assert(m_indexData.empty());
	std::uint32_t lastRestartIndex = std::numeric_limits<std::uint32_t>::max();
	IndexData* indexData = nullptr;
	unsigned int sizeofIndex = indexSize(m_indexType);
	if (m_indexType != IndexType::NoIndices)
	{
		m_indexData.push_back(IndexData{nullptr, m_indexType, 0, 0});
		indexData = &m_indexData.back();
	}

	unsigned int indexStride = primitiveIndexStride(m_primitiveType, m_patchPoints);
	for (std::uint32_t i = 0; i < m_indexCount; i += indexStride)
	{
		// Check if there's room for a new primitive.
		auto vertexCount =
			static_cast<std::uint32_t>(m_vertices[0].size()/m_vertexFormat[0].stride());
		if (m_indexType != IndexType::NoIndices &&
			vertexCount + indexStride - 1 - indexData->baseVertex > m_maxIndexValue)
		{
			std::int32_t baseVertex = vertexCount;
			m_indexData.push_back(IndexData{reinterpret_cast<void*>(m_indices.size()),
				m_indexType, 0, baseVertex});
			indexData = &m_indexData.back();
			vertexSet.clear();

			// Copy any vertices that are needed.
			assert(m_indexData.size() >= 2);
			IndexData& lastIndexData = m_indexData[m_indexData.size() - 2];
			auto indexCount = static_cast<std::uint32_t>(m_indices.size()/sizeofIndex);
			copyConnectedVertices(m_vertices, m_vertexFormat, vertexSet, m_indices,
				m_indexType, sizeofIndex, baseVertex, m_primitiveType, lastRestartIndex,
				lastIndexData.count, lastIndexData.baseVertex, indexData->count);
			// Count this as a the first index after a primitive restart.
			lastRestartIndex = indexCount - 1;
		}

		for (std::uint32_t j = 0; j < indexStride; ++j)
		{
			std::uint32_t index = i + j;
			bool restart = false;
			for (std::size_t k = 0; k < m_elementMapping.size(); ++k)
			{
				const VertexFormat& curFormat = m_vertexFormat[k];
				const std::vector<VertexElementRef>& curElementMapping = m_elementMapping[k];
				for (std::size_t l = 0; l < curFormat.size(); ++l)
				{
					const VertexElementRef& elementRef = curElementMapping[l];
					const VertexStream& stream = m_vertexStreams[elementRef.streamIndex];
					assert(elementRef.element);
					const VertexElement& element = *elementRef.element;
					const VertexElement& dstElement = curFormat[l];

					// Handle primitive restart.
					std::uint32_t indexValue =
						getIndexValue(stream.indexType, stream.indexData, index, index);
					std::uint32_t primitiveRestart = primitiveRestartIndexValue(stream.indexType);
					if (isPrimitiveRestart(indexValue, primitiveRestart, m_primitiveType))
					{
						assert(m_indexType != IndexType::NoIndices);
						restart = true;
						break;
					}
					assert(indexValue < stream.vertexCount);

					// Read the current element.
					VertexValue value;
					auto offset = static_cast<std::size_t>(indexValue)*stream.vertexFormat.stride() +
						element.offset;
					value.fromData(stream.vertexData + offset, element.layout, element.type);

					// Then write it into the combined vertex.
					std::uint8_t* elementPtr = vertexData[k].data() + dstElement.offset;
					switch (elementRef.transform)
					{
						case Transform::Identity:
							value.toData(elementPtr, dstElement.layout, dstElement.type);
							break;
						case Transform::Bounds:
							value.toData(elementPtr, dstElement.layout, dstElement.type,
								elementRef.minVal, elementRef.maxVal);
							break;
						case Transform::UNormToSNorm:
							for (unsigned int m = 0; m < VertexValue::count; ++m)
								value[m] = value[m]*2 - 1.0;
							value.toData(elementPtr, dstElement.layout, dstElement.type);
							break;
						case Transform::SNormToUNorm:
							for (unsigned int m = 0; m < VertexValue::count; ++m)
								value[m] = value[m]*0.5 + 0.5;
							value.toData(elementPtr, dstElement.layout, dstElement.type);
							break;
						default:
							assert(false);
							break;
					}
				}
			}

			if (restart)
			{
				assert(indexStride == 1);
				assert(m_indexType != IndexType::NoIndices);
				lastRestartIndex = static_cast<std::uint32_t>(m_indices.size()/sizeofIndex);
				addIndex(m_indices, m_indexType, sizeofIndex,
					primitiveRestartIndexValue(m_indexType));
				++indexData->count;
				break; // Continues outer loop.
			}

			// Add the vertex and index once all the data has been added.
			if (m_indexType == IndexType::NoIndices)
			{
				for (std::size_t i = 0; i < m_vertices.size(); ++i)
				{
					m_vertices[i].insert(m_vertices[i].end(), vertexData[i].begin(),
						vertexData[i].end());
				}
			}
			else
			{
				assert(indexData);
				std::uint32_t vertexIndex =
					addVertex(m_vertices, m_vertexFormat, vertexData, vertexSet);
				std::uint32_t indexValue = vertexIndex - indexData->baseVertex;
				assert(indexValue <= m_maxIndexValue);
				addIndex(m_indices, m_indexType, sizeofIndex, indexValue);
				++indexData->count;
			}
		}
	}

	// Set the pointers for the index data.
	for (IndexData& indexData : m_indexData)
		indexData.data = m_indices.data() + reinterpret_cast<std::size_t>(indexData.data);

	return true;
}

bool Converter::getVertexElementBounds(VertexValue& outMin, VertexValue& outMax,
	const char* name) const
{
	for (std::size_t i = 0; i < m_vertexFormat.size(); ++i)
	{
		const VertexFormat& curFormat = m_vertexFormat[i];
		auto it = curFormat.find(name);
		if (it == curFormat.end())
			continue;

		getVertexElementBounds(outMin, outMax, i, it - curFormat.begin());
		return true;
	}

	return false;
}

} // namespace vfc
