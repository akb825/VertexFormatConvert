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
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

/**
 * @file
 * @brief Functions and type for working with the vertex format.
 */

namespace vfc
{

/**
 * @brief Enum for the element layout within a vertex format.
 */
enum class ElementLayout : std::int8_t
{
	Invalid = -1,     ///< Invalid layout.
	X8,				  ///< X with 8 bits.
	X8Y8,			  ///< XY with 8 bits each.
	X8Y8Z8,			  ///< XYZ with 8 bits each.
	X8Y8Z8W8,		  ///< XYZW with 8 bits each.
	W2X10Y10Z10,	  ///< WXYZ with 2, 10, 10, 10 bits.
	W2Z10Y10X10,	  ///< WZYX with 2, 10, 10, 10 bits.
	X16,			  ///< X with 16 bits.
	X16Y16,			  ///< XY with 16 bits each.
	X16Y16Z16,		  ///< XYZ with 16 bits each.
	X16Y16Z16W16,	  ///< XYZW with 16 bits each.
	X32,			  ///< X with 32 bits.
	X32Y32,			  ///< XY with 32 bits each.
	X32Y32Z32,		  ///< XYZ with 32 bits each.
	X32Y32Z32W32,	  ///< XYZW with 32 bits each.
	X64,			  ///< X with 64 bits.
	X64Y64,			  ///< XY with 64 bits each.
	X64Y64Z64,		  ///< XYZ with 64 bits each.
	X64Y64Z64W64,	  ///< XYZW with 64 bits each.
	Z10Y11X11_UFloat, ///< ZYX with 10, 11, 11 bits as unsigned floats.
	E5Z9Y9X9_UFloat,  ///< ZYX with 9 bits each as unsigned floats with 5 bits shared exponent.

	R8 = X8,
	R8G8 = X8Y8,
	R8G8B8 = X8Y8Z8,
	R8G8B8A8 = X8Y8Z8W8,
	A2R10G10B10 = W2X10Y10Z10,
	A2B10G10R10 = W2Z10Y10X10,
	R16 = X16,
	R16G16 = X16Y16,
	R16G16B16 = X16Y16Z16,
	R16G16B16A16 = X16Y16Z16W16,
	R32 = X32,
	R32G32 = X32Y32,
	R32G32B32 = X32Y32Z32,
	R32G32B32A32 = X32Y32Z32W32,
	R64 = X64,
	R64G64 = X64Y64,
	R64G64B64 = X64Y64Z64,
	R64G64B64A64 = X64Y64Z64W64,
	B10G11R11_UFloat = Z10Y11X11_UFloat,
	E5B9G9R9_UFloat = E5Z9Y9X9_UFloat
};

/**
 * @brief The number of ElementLayout enum values.
 */
constexpr auto elementLayoutCount = static_cast<unsigned int>(ElementLayout::E5Z9Y9X9_UFloat) + 1;

/**
 * @brief Enum for the type of an element within a vertex format.
 */
enum class ElementType : std::int8_t
{
	Invalid = -1, ///< Invalid type.
	UNorm,        ///< Integer converted to a float in the range [0, 1].
	SNorm,        ///< Integer converted to a float in the range [-1, 1].
	UInt,         ///< Unsigned integer.
	SInt,         ///< Signed integer.
	Float         ///< Floating point.
};

/**
 * @brief The number of ElementType enum values.
 */
constexpr auto elementTypeCount = static_cast<unsigned int>(ElementType::Float) + 1;

/**
 * @brief Enum for the type of primitive the geometry represents.
 */
enum class PrimitiveType : std::int8_t
{
	Invalid = -1,
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	TriangleFan,
	PatchList
};

/**
 * @brief The number of PrimitiveType enum values.
 */
constexpr auto primitiveTypeCount = static_cast<unsigned int>(PrimitiveType::PatchList) + 1;

/**
 * @brief Gets a string name for an element layout.
 * @param layout The element layout.
 * @param color True if the color (RGBA) names should be used instead.
 * @return The name of the layout or nullptr if the layout is invalid.
 */
VFC_EXPORT const char* elementLayoutName(ElementLayout layout, bool color = false);

/**
 * @brief Gets the element layout from the name.
 * @param name The name of the element. This is the C++ enum identifier as a string, and is
 *     case-insensitive.
 * @return The layout. If the name cannot be found, ElementLayout::Invalid is returned.
 */
VFC_EXPORT ElementLayout elementLayoutFromName(const char* name);

/**
 * @brief Gets a string name for an element type.
 * @param type The element type.
 * @return The name of the type or nullptr if the type is invalid.
 */
VFC_EXPORT const char* elementTypeName(ElementType type);

/**
 * @brief Gets the element type from the name.
 * @param name The name of the type. This is the C++ enum identifier as a string, and is
 *     case-insensitive.
 * @return The type. If the name cannot be found, ElementType::Invalid is returned.
 */
VFC_EXPORT ElementType elementTypeFromName(const char* name);

/**
 * @brief Gets the size of an element.
 * @param layout The element layout.
 * @return The size of the element.
 */
VFC_EXPORT std::uint32_t elementLayoutSize(ElementLayout layout);

/**
 * @brief Checks if an element layout and type is valid.
 * @param layout The element layout.
 * @param type The element type.
 * @return True if the element layout and type is valid.
 */
VFC_EXPORT bool isElementValid(ElementLayout layout, ElementType type);

/**
 * @brief Gets a string name for an primitive type.
 * @param type The primitive type.
 * @return The name of the type or nullptr if the type is invalid.
 */
VFC_EXPORT const char* primitiveTypeName(PrimitiveType type);

/**
 * @brief Gets the primitive type from the name.
 * @param name The name of the type. This is the C++ enum identifier as a string, and is
 *     case-insensitive.
 * @return The type. If the name cannot be found, PrimitiveType::Invalid is returned.
 */
VFC_EXPORT PrimitiveType primitiveTypeFromName(const char* name);

/**
 * @brief Checks whether or not a vertex count is valid.
 * @param primitiveType The primitive type.
 * @param vertexCount The number of vertices.
 * @param patchSize The size of the patch when primitiveType is PrimitiveType::PatchList.
 * @return True if the vertex count is valid.
 */
VFC_EXPORT bool isVertexCountValid(PrimitiveType primitiveType, std::uint32_t vertexCount,
	unsigned int patchSize = 0);

/**
 * @brief Struct describing a vertex element.
 */
struct VertexElement
{
	/**
	 * @brief The name of the element.
	 *
	 * This can be used to correlate elements between multiple formats.
	 */
	std::string name;

	/**
	 * @brief The layout of the elment.
	 */
	ElementLayout layout;

	/**
	 * @brief The type of the element.
	 */
	ElementType type;

	/**
	 * @brief The offset in bytes from the start of the vertex to this element.
	 */
	std::uint32_t offset;

	/**
	 * @brief Checks whether this is the same as another vertex element.
	 * @param other The other format to check.
	 * @return True if the vertex elements are equivalent.
	 */
	bool operator==(const VertexElement& other) const
	{
		return name == other.name && layout == other.layout && type == other.type &&
			offset == other.offset;
	}

	/**
	 * @brief Checks whether this is not the same as another vertex element.
	 * @param other The other element to check.
	 * @return True if the vertex element are not equivalent.
	 */
	bool operator!=(const VertexElement& other) const
	{
		return !(*this == other);
	}
};

/**
 * @brief Class describing a vertex format.
 *
 * The vertex format is a collection of vertex elements that describe the data layout. Elements
 * may be accessed similar to std::vector, and this mimicks the relevant interfce for read-only
 * access.
 */
class VFC_EXPORT VertexFormat
{
	using ElementVector = std::vector<VertexElement>;
public:
	/**
	 * @brief Type for an array value.
	 */
	using value_type = ElementVector::value_type;

	/**
	 * @brief Type for the array size.
	 */
	using size_type = ElementVector::size_type;

	/**
	 * @brief Type for the difference between elements.
	 */
	using difference_type = ElementVector::difference_type;

	/**
	 * @brief Type for a reference to an element.
	 */
	using reference = ElementVector::reference;

	/**
	 * @brief Type for a const reference to an element.
	 */
	using const_reference = ElementVector::const_reference;

	/**
	 * @brief Type for a pointer to an element.
	 */
	using pointer = ElementVector::pointer;

	/**
	 * @brief Type for a const pointer to an element.
	 */
	using const_pointer = ElementVector::const_pointer;

	/**
	 * @brief Type for an iterator over the elements.
	 */
	using iterator = ElementVector::const_iterator;

	/** @copydoc iterator */
	using const_iterator = ElementVector::const_iterator;

	/**
	 * @brief Type for an iterator over the reversed elements.
	 */
	using reverse_iterator = ElementVector::const_reverse_iterator;

	/** @copydoc reverse_iterator */
	using const_reverse_iterator = ElementVector::const_reverse_iterator;

	/**
	 * @brief Enum for the result of adding to the format.
	 */
	enum class AddResult
	{
		Succeeded,     ///< The element was successfully added.
		NameNotUnique, ///< The name was not unique.
		ElementInvalid ///< The element layout and type was invalid.
	};

	VertexFormat()
		: m_stride(0)
	{
	}

	/**
	 * @brief Appends an element to the vertex format.
	 * @param name The name of the element. This must be unique.
	 * @param layout The layout of the element.
	 * @param type The type of the element.
	 * @return The result of adding the element.
	 */
	AddResult appendElement(std::string name, ElementLayout layout, ElementType type);

	/**
	 * @brief Gets the stride of the vertex.
	 * @return The stride in bytes.
	 */
	std::uint32_t stride() const
	{
		return m_stride;
	}

	/**
	 * @brief Finds a vertex element by name.
	 * @param name The name of the elment.
	 * @return The iterator to the element or end() if not found.
	 */
	const_iterator find(const char* name) const;

	/** @copydoc find() */
	const_iterator find(const std::string& name) const
	{
		return find(name.c_str());
	}

	/**
	 * @brief Checks if this vertex format contains the same elements as another format.
	 *
	 * It is valid for this vertex format to contain more elements than other. This will only check
	 * the name.
	 *
	 * @param other The other vertex format to check.
	 * @return True if this contains all the elements in other.
	 */
	bool containsElements(const VertexFormat& other) const;

	/**
	 * @brief Accesses an element by index.
	 * @param i The index to access. std::out_of_range will be thrown if i is out of range.
	 * @return The element at index i.
	 */
	const_reference at(size_type i) const
	{
		return m_elements.at(i);
	}

	/**
	 * @brief Accesses an element by index.
	 * @param i The index to access.
	 * @return The element at index i.
	 */
	const_reference operator[](size_type i) const
	{
		assert(i < m_elements.size());
		return m_elements[i];
	}

	/**
	 * @brief Gets the first element.
	 * @return The first element.
	 */
	const_reference front() const
	{
		assert(!m_elements.empty());
		return m_elements.front();
	}

	/**
	 * @brief Gets the last element.
	 * @return The last element.
	 */
	const_reference back() const
	{
		assert(!m_elements.empty());
		return m_elements.back();
	}

	/**
	 * @brief Gets an iterator to the first element.
	 * @return The begin iterator.
	 */
	const_iterator begin() const
	{
		return m_elements.begin();
	}

	/** @copydoc begin() */
	const_iterator cbegin() const
	{
		return m_elements.cbegin();
	}

	/**
	 * @brief Gets an iterator past the last element.
	 * @return The end iterator.
	 */
	const_iterator end() const
	{
		return m_elements.end();
	}

	/** @copydoc end() */
	const_iterator cend() const
	{
		return m_elements.cend();
	}

	/**
	 * @brief Gets a reversed iterator to the last element.
	 * @return The reversed begin iterator.
	 */
	const_reverse_iterator rbegin() const
	{
		return m_elements.rbegin();
	}

	/** @copydoc rbegin() */
	const_reverse_iterator crbegin() const
	{
		return m_elements.crbegin();
	}

	/**
	 * @brief Gets a reversed iterator past the first element.
	 * @return The end iterator.
	 */
	const_reverse_iterator rend() const
	{
		return m_elements.rend();
	}

	/** @copydoc rend() */
	const_reverse_iterator crend() const
	{
		return m_elements.crend();
	}

	/**
	 * @brief Gets whether or not the elements are empty.
	 * @return True if there are no elements.
	 */
	bool empty() const
	{
		return m_elements.empty();
	}

	/**
	 * @brief Gets the number of elements.
	 * @return The number of elements.
	 */
	size_type size() const
	{
		return m_elements.size();
	}

	/**
	 * @brief Clears the vertex format to its default state.
	 */
	void clear()
	{
		m_elements.clear();
		m_stride = 0;
	}

	/**
	 * @brief Checks whether this is the same as another vertex format.
	 * @param other The other format to check.
	 * @return True if the vertex formats are equivalent.
	 */
	bool operator==(const VertexFormat& other) const
	{
		return m_elements == other.m_elements;
	}

	/**
	 * @brief Checks whether this is not the same as another vertex format.
	 * @param other The other format to check.
	 * @return True if the vertex formats are not equivalent.
	 */
	bool operator!=(const VertexFormat& other) const
	{
		return !(*this == other);
	}

private:
	ElementVector m_elements;
	std::uint32_t m_stride;
};

} // namespace vfc
