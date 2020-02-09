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

#include "ResultFile.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <cassert>

std::string resultFile(const vfc::VertexFormat& vertexFormat, std::uint32_t vertexCount,
	const char* vertexData, vfc::IndexType indexType, const IndexFileData* indexData,
	std::size_t indexDataCount)
{
	assert(indexDataCount == 0 || indexData);
	rapidjson::Document document(rapidjson::kObjectType);

	rapidjson::Value vertexFormatArray(rapidjson::kArrayType);
	vertexFormatArray.Reserve(static_cast<std::uint32_t>(vertexFormat.size()),
		document.GetAllocator());
	for (const vfc::VertexElement& element : vertexFormat)
	{
		rapidjson::Value elementObject(rapidjson::kObjectType);
		elementObject.MemberReserve(4, document.GetAllocator());
		elementObject.AddMember("name", rapidjson::StringRef(element.name.c_str()),
			document.GetAllocator());
		elementObject.AddMember("layout",
			rapidjson::StringRef(vfc::elementLayoutName(element.layout)), document.GetAllocator());
		elementObject.AddMember("type",
			rapidjson::StringRef(vfc::elementTypeName(element.type)), document.GetAllocator());
		elementObject.AddMember("offset", element.offset, document.GetAllocator());
		vertexFormatArray.PushBack(elementObject, document.GetAllocator());
	}
	document.AddMember("vertexFormat", vertexFormatArray, document.GetAllocator());
	document.AddMember("vertexStride", vertexFormat.stride(), document.GetAllocator());
	document.AddMember("vertexCount", vertexCount, document.GetAllocator());
	document.AddMember("vertexData", rapidjson::StringRef(vertexData), document.GetAllocator());

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

	if (indexType != vfc::IndexType::NoIndices && indexDataCount > 0)
	{
		rapidjson::Value indexDataArray(rapidjson::kArrayType);
		indexDataArray.Reserve(static_cast<std::uint32_t>(indexDataCount),
			document.GetAllocator());
		for (std::size_t i = 0; i < indexDataCount; ++i)
		{
			rapidjson::Value indexDataObject(rapidjson::kObjectType);
			indexDataObject.MemberReserve(3, document.GetAllocator());
			indexDataObject.AddMember("indexCount", indexData[i].count, document.GetAllocator());
			indexDataObject.AddMember("baseVertex", indexData[i].baseVertex,
				document.GetAllocator());
			indexDataObject.AddMember("indexData", rapidjson::StringRef(indexData[i].dataFile),
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
