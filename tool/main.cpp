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

#include "ConfigFile.h"
#include "Path.h"
#include "ResultFile.h"

#include <cstdio>

void printHelp(const char* argv0)
{
	std::printf("Usage: %s [OPTIONS]\n\n", path::getFileName(argv0).c_str());
	std::printf("Converts input vertex data to a form more suitable for drawing on the GPU.\n");

	std::printf("\nOptions:\n");
	std::printf("-h, --help          Prints this help message and exits.\n");
	std::printf("-i, --input <file>  Path to a JSON file that defines the input to process. If\n");
	std::printf("                    not provided, input will be read from stdin.\n");
	std::printf("-o, --output <dir>  Path to a directory to output the results to. The directory\n");
	std::printf("                    will be created if it doesn't exist.\n");

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
	std::printf("  - vertexData: The path to a data file containing the vertex data.\n");
	std::printf("  - indexType: (optional) The type of the input index data. If not provided or\n");
	std::printf("    null, index data isn't used.\n");
	std::printf("  - indexData: (required if indexType is set) The path to a data file containing\n");
	std::printf("    the index data.\n");
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
	std::printf("- vertexStride: The size in bytes of each vertex.\n");
	std::printf("- vertexCount: The number of vertices that were output.\n");
	std::printf("- vertexData: The path to a data file containing the output vertices.\n");
	std::printf("- indexType: (set if indexType was set on input) The type of the index data.\n");
	std::printf("- indexBuffers: (set if indexType was set on input) The index buffers that were\n");
	std::printf("  output. It is an array of objects with the following elements:\n");
	std::printf("  - indexCount: The number of indices for this buffer.\n");
	std::printf("  - baseVertex: The value to add to each index value to get the final vertex\n");
	std::printf("    index. This can be applied when drawing the mesh.\n");
	std::printf("  - indexData: The path to a data file containing the output indices.\n");

	std::printf("\nAll output files are placed in the directory provided by the --output\n");
	std::printf("command-line option.\n");
}

int main(int argc, const char** argv)
{
	(void)argc;
	printHelp(argv[0]);
	return 0;
}
