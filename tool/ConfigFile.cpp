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

#include "ConfigFile.h"
#include <rapidjson/document.h>
#include <cassert>
#include <fstream>

#include <string.h>

#if VFC_WINDOWS
#define strcasecmp(x, y) _stricmp(x, y)
#else
#include <strings.h>
#endif

static const char* getErrorString(rapidjson::ParseErrorCode error)
{
	switch (error)
	{
		case rapidjson::kParseErrorNone:
			return "No error.";
		case rapidjson::kParseErrorDocumentEmpty:
			return "Empty document.";
		case rapidjson::kParseErrorDocumentRootNotSingular:
			return "Multiple document roots.";
		case rapidjson::kParseErrorValueInvalid:
			return "Invalid value.";
		case rapidjson::kParseErrorObjectMissName:
			return "Missing name for object member.";
		case rapidjson::kParseErrorObjectMissColon:
			return "Missing ':' after object member name.";
		case rapidjson::kParseErrorObjectMissCommaOrCurlyBracket:
			return "Missing ',' or '}' after object member.";
		case rapidjson::kParseErrorArrayMissCommaOrSquareBracket:
			return "Missing ',' or ']' after array member.";
		case rapidjson::kParseErrorStringUnicodeEscapeInvalidHex:
			return "Invalid digit after \\u escape in string.";
		case rapidjson::kParseErrorStringUnicodeSurrogateInvalid:
			return "Invalid surrogate pair in string.";
		case rapidjson::kParseErrorStringEscapeInvalid:
			return "Invalid escape character.";
		case rapidjson::kParseErrorStringMissQuotationMark:
			return "Missing closing '\"' for string.";
		case rapidjson::kParseErrorStringInvalidEncoding:
			return "Invalid string encoding.";
		case rapidjson::kParseErrorNumberTooBig:
			return "Number is too large to store in a double.";
		case rapidjson::kParseErrorNumberMissFraction:
			return "Missing fraction part in number.";
		case rapidjson::kParseErrorNumberMissExponent:
			return "Missing exponent in number.";
		case rapidjson::kParseErrorTermination:
			return "Parsing was terminated.";
		case rapidjson::kParseErrorUnspecificSyntaxError:
			return "Syntax error.";
	}

	return "Unknown error";
}

static std::pair<unsigned int, unsigned int> getLineColumn(const char* json, std::size_t offset)
{
	std::pair<unsigned int, unsigned int> lineColumn(1, 1);
	for (std::size_t i = 0; i < offset && json[i]; ++i)
	{
		if (json[i] == '\n')
		{
			++lineColumn.first;
			lineColumn.second = 1;
		}
		else
			++lineColumn.second;
	}

	return lineColumn;
}

static std::string errorMessageStart(const char* json, const char* fileName,
	std::size_t errorOffset)
{
	std::pair<unsigned int, unsigned int> lineColumn = getLineColumn(json, errorOffset);
	std::string message = fileName;
#if VFC_WINDOWS
	message += '(';
	message += std::to_string(lineColumn.first);
	message += ", ";
	message += std::to_string(lineColumn.second);
	message += ") :";
#else
	message += ':';
	message += std::to_string(lineColumn.first);
	message += ':';
	message += std::to_string(lineColumn.second);
	message += ':';
#endif

	message += " error: ";
	return message;
}

static vfc::VertexFormat readVertexFormat(const rapidjson::Value& value, const char* fileName,
	const vfc::Converter::ErrorFunction& errorFunction, bool inner = false)
{
	vfc::VertexFormat vertexFormat;
	if (!value.IsArray())
	{
		std::string message = fileName;
		if (inner)
			message += ": error: Vertex format must be an array of arrays.";
		else
			message += ": error: Vertex format must be an array.";
		errorFunction(message.c_str());
		return vertexFormat;
	}

	for (auto it = value.Begin(); it != value.End(); ++it)
	{
		if (!it->IsObject())
		{
			std::string message = fileName;
			message += ": error: Vertex format element must be an object.";
			errorFunction(message.c_str());
			vertexFormat.clear();
			return vertexFormat;
		}

		auto nameIt = it->FindMember("name");
		if (nameIt == it->MemberEnd() || !nameIt->value.IsString())
		{
			std::string message = fileName;
			message += ": error: Vertex format element must contain 'name' string member.";
			errorFunction(message.c_str());
			vertexFormat.clear();
			return vertexFormat;
		}

		auto layoutIt = it->FindMember("layout");
		if (layoutIt == it->MemberEnd() || !layoutIt->value.IsString())
		{
			std::string message = fileName;
			message += ": error: Vertex format element must contain 'layout' string member.";
			errorFunction(message.c_str());
			vertexFormat.clear();
			return vertexFormat;
		}

		vfc::ElementLayout layout = vfc::elementLayoutFromName(layoutIt->value.GetString());
		if (layout == vfc::ElementLayout::Invalid)
		{
			std::string message = fileName;
			message += ": error: Vertex format element layout '";
			message += layoutIt->value.GetString();
			message += "' is invalid.";
			errorFunction(message.c_str());
			vertexFormat.clear();
			return vertexFormat;
		}

		auto typeIt = it->FindMember("type");
		if (typeIt == it->MemberEnd() || !typeIt->value.IsString())
		{
			std::string message = fileName;
			message += ": error: Vertex format element must contain 'type' string member.";
			errorFunction(message.c_str());
			vertexFormat.clear();
			return vertexFormat;
		}

		vfc::ElementType type = vfc::elementTypeFromName(typeIt->value.GetString());
		if (type == vfc::ElementType::Invalid)
		{
			std::string message = fileName;
			message += ": error: Vertex format element type '";
			message += typeIt->value.GetString();
			message += "' is invalid.";
			errorFunction(message.c_str());
			vertexFormat.clear();
			return vertexFormat;
		}

		vfc::VertexFormat::AddResult result =
			vertexFormat.appendElement(nameIt->value.GetString(), layout, type);
		if (result != vfc::VertexFormat::AddResult::Succeeded)
		{
			std::string message = fileName;
			message += ": error: ";
			if (result == vfc::VertexFormat::AddResult::NameNotUnique)
			{
				message += "Vertex format element name '";
				message += nameIt->value.GetString();
				message += "' isn't unique.";
			}
			else
			{
				assert(result == vfc::VertexFormat::AddResult::ElementInvalid);
				message += "Vertex format element layout '";
				message += layoutIt->value.GetString();
				message += "' can't be used with type '";
				message += typeIt->value.GetString();
				message += "'.";
			}

			errorFunction(message.c_str());
			vertexFormat.clear();
			return vertexFormat;
		}
	}

	if (vertexFormat.empty())
	{
		std::string message = fileName;
		if (inner)
			message += ": error: Inner vertex format is empty.";
		else
			message += ": error: Vertex format is empty.";
		errorFunction(message.c_str());
	}

	return vertexFormat;
}

static std::vector<vfc::VertexFormat> readVertexFormatArray(const rapidjson::Value& value,
	const char* fileName, const vfc::Converter::ErrorFunction& errorFunction)
{
	std::vector<vfc::VertexFormat> vertexFormat;
	if (!value.IsArray())
	{
		std::string message = fileName;
		message += ": error: Vertex format must be an array of arrays.";
		errorFunction(message.c_str());
		return vertexFormat;
	}

	vertexFormat.reserve(value.Size());
	for (auto it = value.Begin(); it != value.End(); ++it)
	{
		vertexFormat.push_back(readVertexFormat(*it, fileName, errorFunction, true));
		const vfc::VertexFormat& curFormat = vertexFormat.back();
		if (curFormat.empty())
		{
			vertexFormat.clear();
			return vertexFormat;
		}

		for (const vfc::VertexElement& element : curFormat)
		{
			for (std::size_t i = 0; i < vertexFormat.size() - 1; ++i)
			{
				if (vertexFormat[i].find(element.name.c_str()) == vertexFormat[i].end())
					continue;

				std::string message = fileName;
				message += ": error: Vertex format element name '";
				message += element.name;
				message += "' isn't unique.";
				errorFunction(message.c_str());
				vertexFormat.clear();
				return vertexFormat;
			}
		}
	}

	if (vertexFormat.empty())
	{
		std::string message = fileName;
		message += ": error: Vertex format is empty.";
		errorFunction(message.c_str());
	}

	return vertexFormat;
}

static bool readIndexType(vfc::IndexType& outIndexType, const rapidjson::Value& rootValue,
	const char* fileName, const vfc::Converter::ErrorFunction& errorFunction)
{
	auto indexTypeIt = rootValue.FindMember("indexType");
	if (indexTypeIt == rootValue.MemberEnd() || indexTypeIt->value.IsNull())
	{
		outIndexType = vfc::IndexType::NoIndices;
		return true;
	}

	if (!indexTypeIt->value.IsString())
	{
		std::string message = fileName;
		message += ": error: Index type must be a string.";
		errorFunction(message.c_str());
		return false;
	}

	const char* indexTypeStr = indexTypeIt->value.GetString();
	if (strcasecmp(indexTypeStr, "uint16") == 0)
	{
		outIndexType = vfc::IndexType::UInt16;
		return true;
	}
	else if (strcasecmp(indexTypeStr, "uint32") == 0)
	{
		outIndexType = vfc::IndexType::UInt32;
		return true;
	}
	else
	{
		std::string message = fileName;
		message += ": error: Index type '";
		message += indexTypeStr;
		message += "' is invalid.";
		errorFunction(message.c_str());
		return false;
	}
}

static bool readPrimitiveType(vfc::PrimitiveType& outPrimitiveType, std::uint32_t& outPatchPoints,
	const rapidjson::Value& rootValue, const char* fileName,
	const vfc::Converter::ErrorFunction& errorFunction)
{
	auto primitiveTypeIt = rootValue.FindMember("primitiveType");
	if (primitiveTypeIt == rootValue.MemberEnd() || primitiveTypeIt->value.IsNull())
	{
		outPrimitiveType = vfc::PrimitiveType::TriangleList;
		outPatchPoints = 0;
		return true;
	}

	if (!primitiveTypeIt->value.IsString())
	{
		std::string message = fileName;
		message += ": error: Primitive type must be a string.";
		errorFunction(message.c_str());
		return false;
	}

	const char* primitiveTypeStr = primitiveTypeIt->value.GetString();
	outPrimitiveType = vfc::primitiveTypeFromName(primitiveTypeStr);
	if (outPrimitiveType == vfc::PrimitiveType::Invalid)
	{
		std::string message = fileName;
		message += ": error: Primitive type '";
		message += primitiveTypeStr;
		message += "' is invalid.";
		errorFunction(message.c_str());
		return false;
	}

	if (outPrimitiveType == vfc::PrimitiveType::PatchList)
	{
		auto patchPointsIt = rootValue.FindMember("patchPoints");
		if (patchPointsIt == rootValue.MemberEnd() || !patchPointsIt->value.IsInt())
		{
			std::string message = fileName;
			message += ": error: Root must contain 'patchPoints' int member.";
			errorFunction(message.c_str());
			return false;
		}

		int patchPoints = patchPointsIt->value.GetInt();
		if (patchPoints <= 0)
		{
			std::string message = fileName;
			message += ": error: Patch points must have a value > 0.";
			errorFunction(message.c_str());
			return false;
		}

		outPatchPoints = patchPoints;
	}

	return true;
}

static std::vector<ConfigFile::VertexStream> readVertexStreams(const rapidjson::Value& value,
	const char* fileName, const vfc::Converter::ErrorFunction& errorFunction)
{
	std::vector<ConfigFile::VertexStream> vertexStreams;
	if (!value.IsArray())
	{
		std::string message = fileName;
		message += ": error: Vertex streams must be an array.";
		errorFunction(message.c_str());
		return vertexStreams;
	}

	for (auto it = value.Begin(); it != value.End(); ++it)
	{
		if (!it->IsObject())
		{
			std::string message = fileName;
			message += ": error: Vertex stream element must be an object.";
			errorFunction(message.c_str());
			vertexStreams.clear();
			return vertexStreams;
		}

		vertexStreams.emplace_back();
		ConfigFile::VertexStream& stream = vertexStreams.back();

		auto formatIt = it->FindMember("vertexFormat");
		if (formatIt == it->MemberEnd())
		{
			std::string message = fileName;
			message += ": error: Vertex stream element must contain 'vertexFormat' member.";
			errorFunction(message.c_str());
			vertexStreams.clear();
			return vertexStreams;
		}

		stream.vertexFormat = readVertexFormat(formatIt->value, fileName, errorFunction);
		if (stream.vertexFormat.empty())
		{
			vertexStreams.clear();
			return vertexStreams;
		}

		auto vertexDataIt = it->FindMember("vertexData");
		if (vertexDataIt == it->MemberEnd() || !vertexDataIt->value.IsString())
		{
			std::string message = fileName;
			message += ": error: Vertex stream element must contain 'vertexData' string member.";
			errorFunction(message.c_str());
			vertexStreams.clear();
			return vertexStreams;
		}

		stream.vertexData = vertexDataIt->value.GetString();

		if (!readIndexType(stream.indexType, *it, fileName, errorFunction))
		{
			vertexStreams.clear();
			return vertexStreams;
		}

		if (stream.indexType != vfc::IndexType::NoIndices)
		{
			auto indexDataIt = it->FindMember("indexData");
			if (indexDataIt == it->MemberEnd() || !indexDataIt->value.IsString())
			{
				std::string message = fileName;
				message += ": error: Vertex stream element must contain 'indexData' string member.";
				errorFunction(message.c_str());
				vertexStreams.clear();
				return vertexStreams;
			}

			stream.indexData = indexDataIt->value.GetString();
		}
	}

	if (vertexStreams.empty())
	{
		std::string message = fileName;
		message += ": error: Vertex streams are empty.";
		errorFunction(message.c_str());
	}

	return vertexStreams;
}

static bool readVertexTransforms(
	std::vector<std::pair<std::string, vfc::Converter::Transform>>& outTransforms,
	const rapidjson::Value& value, const char* fileName,
	const vfc::Converter::ErrorFunction& errorFunction)
{
	if (!value.IsArray())
	{
		std::string message = fileName;
		message += ": error: Vertex transforms must be an array.";
		errorFunction(message.c_str());
		return false;
	}

	for (auto it = value.Begin(); it != value.End(); ++it)
	{
		if (!it->IsObject())
		{
			std::string message = fileName;
			message += ": error: Vertex transform element must be an object.";
			errorFunction(message.c_str());
			return false;
		}

		auto nameIt = it->FindMember("name");
		if (nameIt == it->MemberEnd() || !nameIt->value.IsString())
		{
			std::string message = fileName;
			message += ": error: Vertex transform element must contain 'name' string member.";
			errorFunction(message.c_str());
			return false;
		}

		auto transformIt = it->FindMember("transform");
		if (transformIt == it->MemberEnd() || !transformIt->value.IsString())
		{
			std::string message = fileName;
			message += ": error: Vertex transform element must contain 'transform' string member.";
			errorFunction(message.c_str());
			return false;
		}

		const char* transformStr = transformIt->value.GetString();
		vfc::Converter::Transform transform;
		if (strcasecmp(transformStr, "identity") == 0)
			transform = vfc::Converter::Transform::Identity;
		else if (strcasecmp(transformStr, "bounds") == 0)
			transform = vfc::Converter::Transform::Bounds;
		else if (strcasecmp(transformStr, "unormtosnorm") == 0)
			transform = vfc::Converter::Transform::UNormToSNorm;
		else if (strcasecmp(transformStr, "snormtounorm") == 0)
			transform = vfc::Converter::Transform::SNormToUNorm;
		else
		{
			std::string message = fileName;
			message += ": error: Vertex transform '";
			message += transformStr;
			message += "' is invalid.";
			errorFunction(message.c_str());
			return false;
		}

		outTransforms.emplace_back(nameIt->value.GetString(), transform);
	}

	return true;
}

bool ConfigFile::load(const char* fileName, const vfc::Converter::ErrorFunction& errorFunction)
{
	assert(errorFunction);
	std::ifstream stream(fileName);
	if (!stream.is_open())
	{
		std::string message = "error: Couldn't open config file '";
		message += fileName;
		message += "'.";
		errorFunction(message.c_str());
		return false;
	}

	return load(stream, fileName, errorFunction);
}

bool ConfigFile::load(std::istream& stream, const char* fileName,
	const vfc::Converter::ErrorFunction& errorFunction)
{
	std::string json(std::istreambuf_iterator<char>(stream), {});
	return load(json.c_str(), fileName, errorFunction);
}

bool ConfigFile::load(const char* json, const char* fileName,
	const vfc::Converter::ErrorFunction& errorFunction)
{
	assert(errorFunction);
	rapidjson::Document document;
	if (document.Parse(json).HasParseError())
	{
		std::string message = errorMessageStart(json, fileName, document.GetErrorOffset());
		message += getErrorString(document.GetParseError());
		errorFunction(message.c_str());
		return false;
	}

	if (!document.IsObject())
	{
		std::string message = fileName;
		message += ": error: Root element must be an object.";
		errorFunction(message.c_str());
		return false;
	}

	auto vertexFormatIt = document.FindMember("vertexFormat");
	if (vertexFormatIt == document.MemberEnd())
	{
		std::string message = fileName;
		message += ": error: Root must contain 'vertexFormat' member.";
		errorFunction(message.c_str());
		return false;
	}

	m_vertexFormat = readVertexFormatArray(vertexFormatIt->value, fileName, errorFunction);
	if (m_vertexFormat.empty())
		return false;

	if (!readIndexType(m_indexType, document, fileName, errorFunction))
		return false;

	if (!readPrimitiveType(m_primitiveType, m_patchPoints, document, fileName, errorFunction))
		return false;

	auto vertexStreamsIt = document.FindMember("vertexStreams");
	if (vertexStreamsIt == document.MemberEnd())
	{
		std::string message = fileName;
		message += ": error: Root must contain 'vertexStreams' member.";
		errorFunction(message.c_str());
		return false;
	}

	m_vertexStreams = readVertexStreams(vertexStreamsIt->value, fileName, errorFunction);
	if (m_vertexStreams.empty())
		return false;

	auto vertexTransformIt = document.FindMember("vertexTransforms");
	if (vertexTransformIt != document.MemberEnd() &&
		!readVertexTransforms(m_transforms, vertexTransformIt->value, fileName, errorFunction))
	{
		return false;
	}

	return true;
}
