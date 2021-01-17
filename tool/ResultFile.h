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

#pragma once

#include <VFC/Config.h>
#include <VFC/IndexData.h>
#include <VFC/VertexFormat.h>
#include <VFC/VertexValue.h>

struct IndexFileData
{
	std::uint32_t count;
	std::int32_t baseVertex;
	const char* dataFile;
};

struct Bounds
{
	vfc::VertexValue min;
	vfc::VertexValue max;
};

std::string resultFile(const std::vector<vfc::VertexFormat>& vertexFormat,
	const std::vector<std::vector<Bounds>>& bounds, const std::vector<std::string>& vertexData,
	std::uint32_t vertexCount, vfc::IndexType indexType,
	const std::vector<IndexFileData>& indexData);
