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

#include "ResultFile.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <cassert>

std::string resultFile(const std::vector<vfc::VertexFormat>& vertexFormat,
	const std::vector<std::vector<Bounds>>& bounds, const std::vector<std::string>& vertexData,
	std::uint32_t vertexCount, vfc::IndexType indexType,
	const std::vector<IndexFileData>& indexData)
{
	assert(vertexFormat.size() == vertexData.size());
	assert(vertexFormat.size() == bounds.size());
	rapidjson::Document document(rapidjson::kObjectType);

	rapidjson::Value vertexArray(rapidjson::kArrayType);
	vertexArray.Reserve(static_cast<std::uint32_t>(vertexFormat.size()),
		document.GetAllocator());
	for (std::size_t i = 0; i < vertexFormat.size(); ++i)
	{
		const vfc::VertexFormat& curFormat = vertexFormat[i];
		const std::vector<Bounds>& curBounds = bounds[i];
		const std::string& curData = vertexData[i];
		assert(curFormat.size() == curBounds.size());

		rapidjson::Value vertexInfo(rapidjson::kObjectType);
		rapidjson::Value vertexFormatArray(rapidjson::kArrayType);
		vertexFormatArray.Reserve(static_cast<std::uint32_t>(vertexFormat.size()),
			document.GetAllocator());
		for (std::size_t j = 0; j < curFormat.size(); ++j)
		{
			const vfc::VertexElement& element = curFormat[j];
			rapidjson::Value elementObject(rapidjson::kObjectType);
			elementObject.MemberReserve(6, document.GetAllocator());
			elementObject.AddMember("name", rapidjson::StringRef(element.name.c_str()),
				document.GetAllocator());
			elementObject.AddMember("layout",
				rapidjson::StringRef(vfc::elementLayoutName(element.layout)), document.GetAllocator());
			elementObject.AddMember("type",
				rapidjson::StringRef(vfc::elementTypeName(element.type)), document.GetAllocator());
			elementObject.AddMember("offset", element.offset, document.GetAllocator());

			rapidjson::Value minBoundsArray(rapidjson::kArrayType);
			rapidjson::Value maxBoundsArray(rapidjson::kArrayType);
			minBoundsArray.Reserve(4, document.GetAllocator());
			maxBoundsArray.Reserve(4, document.GetAllocator());
			for (unsigned int k = 0; k < 4; ++k)
			{
				minBoundsArray.PushBack(curBounds[j].min[k], document.GetAllocator());
				maxBoundsArray.PushBack(curBounds[j].max[k], document.GetAllocator());
			}
			elementObject.AddMember("minValue", minBoundsArray, document.GetAllocator());
			elementObject.AddMember("maxValue", maxBoundsArray, document.GetAllocator());

			vertexFormatArray.PushBack(elementObject, document.GetAllocator());
		}
		vertexInfo.AddMember("vertexFormat", vertexFormatArray, document.GetAllocator());
		vertexInfo.AddMember("vertexStride", curFormat.stride(), document.GetAllocator());
		vertexInfo.AddMember("vertexData", rapidjson::StringRef(curData.c_str()),
			document.GetAllocator());
		vertexArray.PushBack(vertexInfo, document.GetAllocator());
	}
	document.AddMember("vertices", vertexArray, document.GetAllocator());
	document.AddMember("vertexCount", vertexCount, document.GetAllocator());

	switch (indexType)
	{
		case vfc::IndexType::UInt16:
			document.AddMember("indexType", "UInt16", document.GetAllocator());
			break;
		case vfc::IndexType::UInt32:
			document.AddMember("indexType", "UInt32", document.GetAllocator());
			break;
		default:
			break;
	}

	if (indexType != vfc::IndexType::NoIndices && !indexData.empty())
	{
		rapidjson::Value indexDataArray(rapidjson::kArrayType);
		indexDataArray.Reserve(static_cast<std::uint32_t>(indexData.size()),
			document.GetAllocator());
		for (const IndexFileData& curData : indexData)
		{
			rapidjson::Value indexDataObject(rapidjson::kObjectType);
			indexDataObject.MemberReserve(3, document.GetAllocator());
			indexDataObject.AddMember("indexCount", curData.count, document.GetAllocator());
			indexDataObject.AddMember("baseVertex", curData.baseVertex,
				document.GetAllocator());
			indexDataObject.AddMember("indexData", rapidjson::StringRef(curData.dataFile),
				document.GetAllocator());
			indexDataArray.PushBack(indexDataObject, document.GetAllocator());
		}
		document.AddMember("indexBuffers", indexDataArray, document.GetAllocator());
	}

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);
	return buffer.GetString();
}
