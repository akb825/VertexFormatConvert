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

#include "Base64.h"
#include "ConfigFile.h"
#include "Path.h"
#include "ResultFile.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

#include <errno.h>
#include <sys/stat.h>

#if VFC_WINDOWS
#include <direct.h>
#include <string.h>
#define mkdir(path, mode) _mkdir(path)
#define strncasecmp(x, y, n) _strnicmp(x, y, n)
#else
#include <string.h>
#endif

void printHelp(const char* argv0)
{
	std::printf("Usage: %s [OPTIONS]\n\n", path::getFileName(argv0).c_str());
	std::printf("Converts input vertex data to a form more suitable for drawing on the GPU.\n");

	std::printf("\nOptions:\n");
	std::printf("-h, --help          Prints this help message and exits.\n");
	std::printf("-i, --input <file>  Path to a JSON file that defines the input to process. If\n");
	std::printf("                    not provided, input will be read from stdin.\n");
	std::printf("-o, --output <dir>  Path to a directory to output the results to. The directory\n");
	std::printf("                    will be created if it doesn't exist. If not provided, data\n");
	std::printf("                    will be embedded directly in the output JSON with base64\n");
	std::printf("                    encoding.\n");

	std::printf("\nInput:\n");
	std::printf("The primary input is in the form of a JSON configuration. The file has the\n");
	std::printf("following structure, with members required unless otherwise stated:\n");
	std::printf("- vertexFormat: The vertex format to convert to. It is an array of objects with\n");
	std::printf("  the following members:\n");
	std::printf("  - name: The name of the element.\n");
	std::printf("  - layout: The data layout of the element (described below).\n");
	std::printf("  - type: The data type of the element (described below).\n");
	std::printf("- indexType: (optional) The type of the index to output to (described below). If\n");
	std::printf("  not provided or null, no indices will be produced.\n");
	std::printf("- primitiveType: (optional) The type of the primitive (described below). If not\n");
	std::printf("  provided, TriangleList will be assumed.\n");
	std::printf("- patchPoints: (required for PatchList primitive type) The number of patch\n");
	std::printf("  points when the primitive type is PatchList.\n");
	std::printf("- vertexStreams: The input vertex streams to read data from. It is an array of\n");
	std::printf("  objects with the following members:\n");
	std::printf("  - vertexFormat: The vertex format of the vertex stream. See the above\n");
	std::printf("    vertexFormat layout description for details.\n");
	std::printf("  - vertexData: The path to a data file or base64 encoded vertex data.\n");
	std::printf("  - indexType: (optional) The type of the input index data. If not provided or\n");
	std::printf("    null, index data isn't used.\n");
	std::printf("  - indexData: (required if indexType is set) The path to a data file or base64\n");
	std::printf("    encoded index data.\n");
	std::printf("- vertexTransforms: (optional) The transforms to apply to vertex data on\n");
	std::printf("  conversion. It is an array of objects with the following members:\n");
	std::printf("  - name: The name of the element.\n");
	std::printf("  - transform: The transform to apply (described below).\n");

	std::printf("\nGeneral notes on input:\n");
	std::printf("- Names for enums (e.g. layout, type) are case-insensitive. However, names\n");
	std::printf("  provided by 'name' elements are case sensitive when matching with each-other.\n");
	std::printf("- File paths may either be absolute or relative to the input json file. When\n");
	std::printf("  with stdin, the current working directory is used for relative paths.\n");
	std::printf("- Data files are binary files that contain the raw data as described by the\n");
	std::printf("  vertex format for index type. The size is expected to match exactly the vertex\n");
	std::printf("  or index type multiplied by the number of elements.\n");
	std::printf("- If the vertexData or indexData string starts with 'base64:', the rest of the\n");
	std::printf("  string is the base64-encoded data rather than a path to a file.\n");

	std::printf("\nSupported vertex layouts:\n");
	for (unsigned int i = 0; i < vfc::elementLayoutCount; ++i)
		std::printf("- %s\n", vfc::elementLayoutName(static_cast<vfc::ElementLayout>(i)));
	std::printf("Note: RGBA may also be used in place of XYZW.\n");

	std::printf("\nSupported vertex types:\n");
	for (unsigned int i = 0; i < vfc::elementTypeCount; ++i)
		std::printf("- %s\n", vfc::elementTypeName(static_cast<vfc::ElementType>(i)));

	std::printf("\nSupported index types:\n");
	std::printf("- UInt16\n");
	std::printf("- UInt32\n");

	std::printf("\nSupported primitive types:\n");
	for (unsigned int i = 0; i < vfc::primitiveTypeCount; ++i)
		std::printf("- %s\n", vfc::primitiveTypeName(static_cast<vfc::PrimitiveType>(i)));

	std::printf("\nOutput:\n");
	std::printf("The general output is printed to stdout as JSON with the following layout:\n");
	std::printf("- vertexFormat: The verex format that was output. It is an array of objects with\n");
	std::printf("  the following members:\n");
	std::printf("  - name: The name of the element.\n");
	std::printf("  - layout: The data layout of the element.\n");
	std::printf("  - type: The data type of the element.\n");
	std::printf("  - offset: The offset in bytes from the start of the vertex to the element.\n");
	std::printf("  - minValue: The minimum vertex value for this element as 4-element array.\n");
	std::printf("  - maxValue: The maximum vertex value for this element as 4-element array.\n");
	std::printf("- vertexStride: The size in bytes of each vertex.\n");
	std::printf("- vertexCount: The number of vertices that were output.\n");
	std::printf("- vertexData: The path to a data file or base64 encoded output vertices.\n");
	std::printf("- indexType: (set if indexType was set on input) The type of the index data.\n");
	std::printf("- indexBuffers: (set if indexType was set on input) The index buffers that were\n");
	std::printf("  output. It is an array of objects with the following elements:\n");
	std::printf("  - indexCount: The number of indices for this buffer.\n");
	std::printf("  - baseVertex: The value to add to each index value to get the final vertex\n");
	std::printf("    index. This can be applied when drawing the mesh.\n");
	std::printf("  - indexData: The path to a data file or base 64 encoded output indices.\n");

	std::printf("\nAll output files are placed in the directory provided by the --output command-\n");
	std::printf("line option.\n");
}

bool mkdirRecursive(const std::string& directory)
{
	std::string parentDir = path::getParentDirectory(directory);
	if (parentDir == directory)
		return true;

	if (!parentDir.empty() && !mkdirRecursive(parentDir))
		return false;

	return mkdir(directory.c_str(), 0755) == 0 || errno == EEXIST;
}

const char* base64EncodedString(const std::string& dataStr)
{
	const char* prefix = "base64:";
	const std::size_t prefixLen = 7;
	const char* dataCStr = dataStr.c_str();
	if (strncasecmp(dataCStr, prefix, prefixLen) != 0)
		return nullptr;

	return dataCStr + prefixLen;
}

bool loadData(std::vector<std::uint8_t>& outData,  const std::string& configFilePath,
	const std::string& configFileDir, const std::string& dataStr, const char* dataType)
{
	const char* base64Str = base64EncodedString(dataStr);
	if (base64Str)
	{
		if (!base64::decode(outData, base64Str))
		{
			std::fprintf(stderr, "%s: error: Invalid base64 encoding for %s data.\n",
				configFilePath.c_str(), dataType);
			return false;
		}
		return true;
	}

	std::string dataFilePath = path::join(configFileDir, dataStr);
	std::ifstream stream(dataFilePath, std::ios_base::in | std::ios_base::binary);
	if (stream.is_open())
	{
		outData.insert(outData.end(), std::istreambuf_iterator<char>(stream), {});
		if (stream.good() || stream.eof())
			return true;
	}

	std::fprintf(stderr, "%s: error: Couldn't read %s data file '%s'.\n",
		configFilePath.c_str(), dataType, dataFilePath.c_str());
	return false;
}

bool setupConverter(vfc::Converter& converter, const ConfigFile& configFile,
	const std::string& configFilePath, const std::string& configFileDir,
	std::vector<std::vector<std::uint8_t>>& storage)
{
	for (const ConfigFile::VertexStream& vertexStream : configFile.getVertexStreams())
	{
		std::vector<std::uint8_t> vertexData;
		if (!loadData(vertexData, configFilePath, configFileDir, vertexStream.vertexData, "vertex"))
			return false;

		if (vertexData.size() % vertexStream.vertexFormat.stride() != 0)
		{
			std::fprintf(stderr,
				"%s: error: Vertex data isn't divisible by the vertex format size.\n",
				configFilePath.c_str());
			return false;
		}

		std::vector<std::uint8_t> indexData;
		unsigned int indexSize = 1; // Avoid divide by 0 when no indices.
		if (vertexStream.indexType != vfc::IndexType::NoIndices)
		{
			if (!loadData(
					indexData, configFilePath, configFileDir, vertexStream.indexData, "index"))
			{
				return false;
			}

			indexSize = vfc::indexSize(vertexStream.indexType);
			if (indexData.size() % indexSize != 0)
			{
				std::fprintf(stderr,
					"%s: error: Index data isn't divisible by the index format size.\n",
					configFilePath.c_str());
				return false;
			}
		}

		auto vertexCount =
			static_cast<std::uint32_t>(vertexData.size()/vertexStream.vertexFormat.stride());
		auto indexCount = static_cast<std::uint32_t>(indexData.size()/indexSize);
		if (!converter.addVertexStream(vertexStream.vertexFormat, vertexData.data(), vertexCount,
				vertexStream.indexType, indexData.data(), indexCount))
		{
			return false;
		}

		storage.push_back(std::move(vertexData));
		if (!indexData.empty())
			storage.push_back(std::move(indexData));
	}

	for (const auto& transform : configFile.getTransforms())
	{
		if (!converter.setElementTransform(transform.first, transform.second))
		{
			std::fprintf(stderr,
				"%s: error: No vertex element '%s' found for vertex format.\n",
				configFilePath.c_str(), transform.first.c_str());
			return false;
		}
	}

	return true;
}

bool writeFile(const void* data, std::size_t size, const std::string& fileName)
{
	std::ofstream stream(fileName,
		std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	if (!stream.is_open())
		return false;

	stream.write(reinterpret_cast<const char*>(data), size);
	return stream.good();
}

std::string writeOutput(const vfc::Converter& converter, const std::string& outputDir)
{
	const std::vector<std::uint8_t>& vertices = converter.getVertices();
	std::string vertexData;
	if (outputDir.empty())
	{
		vertexData = "base64:";
		vertexData.append(base64::encode(vertices.data(), vertices.size()));
	}
	else
	{
		vertexData = path::join(outputDir, "vertices.dat");
		if (!writeFile(vertices.data(), vertices.size(), vertexData))
		{
			std::fprintf(stderr, "error: Couldn't write vertex output file '%s'.\n",
				vertexData.c_str());
			return "";
		}
	}

	const vfc::VertexFormat& vertexFormat = converter.getVertexFormat();
	std::vector<Bounds> bounds(vertexFormat.size());
	for (std::size_t i = 0; i < vertexFormat.size(); ++i)
		converter.getVertexElementBounds(bounds[i].min, bounds[i].max, i);

	std::vector<IndexFileData> indexFileData;
	std::vector<std::string> indexStrings;
	indexFileData.reserve(converter.getIndices().size());
	indexStrings.reserve(indexFileData.size());
	if (outputDir.empty())
	{
		for (const vfc::IndexData& indexData : converter.getIndices())
		{
			std::size_t indexSize =
				static_cast<std::size_t>(indexData.count)*vfc::indexSize(indexData.type);
			std::string encodedData = "base64:";
			encodedData.append(base64::encode(indexData.data, indexSize));
			indexFileData.push_back(
				IndexFileData{indexData.count, indexData.baseVertex, encodedData.c_str()});
			indexStrings.push_back(std::move(encodedData));
		}
	}
	else
	{
		std::string fileName;
		for (const vfc::IndexData& indexData : converter.getIndices())
		{
			fileName = "indices.";
			fileName += std::to_string(indexFileData.size());
			fileName += ".dat";
			std::string indexDataPath = path::join(outputDir, fileName);
			std::size_t indexSize =
				static_cast<std::size_t>(indexData.count)*vfc::indexSize(indexData.type);
			if (!writeFile(indexData.data, indexSize, indexDataPath))
			{
				std::fprintf(stderr, "error: Couldn't write index output file '%s'.\n",
					indexDataPath.c_str());
				return "";
			}

			indexFileData.push_back(
				IndexFileData{indexData.count, indexData.baseVertex, indexDataPath.c_str()});
			indexStrings.push_back(std::move(indexDataPath));
		}
	}

	return resultFile(converter.getVertexFormat(), bounds.data(), converter.getVertexCount(),
		vertexData.c_str(), converter.getIndexType(), indexFileData.data(),
		indexFileData.size());
}

int main(int argc, const char** argv)
{
	if (argc == 1)
	{
		printHelp(argv[0]);
		return 1;
	}

	std::string input;
	std::string output;
	for (int i = 1; i < argc; ++i)
	{
		if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0)
		{
			printHelp(argv[0]);
			return 0;
		}
		else if (std::strcmp(argv[i], "-i") == 0 || std::strcmp(argv[i], "--input") == 0)
		{
			if (i == argc - 1)
			{
				std::fprintf(stderr, "error: --input requires an argument.\n");
				return 1;
			}

			input = argv[++i];
		}
		else if (std::strcmp(argv[i], "-o") == 0 || std::strcmp(argv[i], "--output") == 0)
		{
			if (i == argc - 1)
			{
				std::fprintf(stderr, "error: --output requires an argument.\n");
				return 1;
			}

			output = argv[++i];
		}
		else
		{
			std::fprintf(stderr, "error: Unknown argument '%s'.\n", argv[i]);
			return 1;
		}
	}

	ConfigFile configFile;
	std::string configFileDir;
	bool configLoadResult = false;
	if (input.empty())
	{
		configLoadResult = configFile.load(std::cin, "stdin");
		input = "stdin";
	}
	else
	{
		configLoadResult = configFile.load(input.c_str());
		configFileDir = path::getParentDirectory(input.c_str());
	}
	if (!configLoadResult)
		return 1;

	vfc::Converter converter(configFile.getVertexFormat(), configFile.getIndexType(),
		configFile.getPrimitiveType(), configFile.getPatchPoints(),
		[&input](const char* message)
		{
			std::fprintf(stderr, "%s: error: %s\n", input.c_str(), message);
		});
	std::vector<std::vector<std::uint8_t>> storage;
	if (!converter || !setupConverter(converter, configFile, input, configFileDir, storage))
		return 1;

	// Config file can contain base64 data, so clear out memory.
	configFile = ConfigFile();

	if (!output.empty() && !mkdirRecursive(output))
	{
		std::fprintf(stderr, "error: Couldn't create output path '%s'.\n", output.c_str());
		return 1;
	}

	if (!converter.convert())
		return 1;

	std::string result = writeOutput(converter, output);
	if (result.empty())
		return 1;

	std::printf("%s\n", result.c_str());
	return 0;
}
