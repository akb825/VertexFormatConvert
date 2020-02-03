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
#include <VFC/Converter.h>
#include <VFC/VertexFormat.h>
#include <VFC/IndexData.h>
#include <string>
#include <vector>
#include <utility>

class ConfigFile
{
public:
	struct VertexStream
	{
		vfc::VertexFormat vertexFormat;
		vfc::IndexType indexType;
		std::string vertexData;
		std::string indexData;

		bool operator==(const VertexStream& other) const
		{
			return vertexFormat == other.vertexFormat && indexType == other.indexType &&
				vertexData == other.vertexData && indexData == other.indexData;
		}

		bool operator!=(const VertexStream& other) const
		{
			return !(*this == other);
		}
	};

	bool load(const char* fileName,
		const vfc::Converter::ErrorFunction& errorFunction = &vfc::Converter::stderrErrorFunction);

	bool load(const char* json, const char* fileName,
		const vfc::Converter::ErrorFunction& errorFunction = &vfc::Converter::stderrErrorFunction);

	const vfc::VertexFormat& getVertexFormat() const
	{
		return m_vertexFormat;
	}

	vfc::IndexType getIndexType() const
	{
		return m_indexType;
	}

	const std::vector<VertexStream>& getVertexStreams() const
	{
		return m_vertexStreams;
	}

	const std::vector<std::pair<std::string, vfc::Converter::Transform>>& getTransforms() const
	{
		return m_transforms;
	}

private:
	vfc::VertexFormat m_vertexFormat;
	vfc::IndexType m_indexType = vfc::IndexType::NoIndices;
	std::vector<VertexStream> m_vertexStreams;
	std::vector<std::pair<std::string, vfc::Converter::Transform>> m_transforms;
};
