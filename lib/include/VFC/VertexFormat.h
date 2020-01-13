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
#include <cstddef>

/**
 * @file
 * @brief Functions and type for working with the vertex format.
 */

namespace vfc
{

/**
 * @brief Enum for the element layout within a vertex format.
 */
enum class ElementLayout
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
enum class ElementType
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
VFC_EXPORT std::size_t elementLayoutSize(ElementLayout layout);

/**
 * @brief Checks if an element layout and type is valid.
 * @param layout The element layout.
 * @param type The element type.
 * @return True if the element layout and type is valid.
 */
VFC_EXPORT bool isElementValid(ElementLayout layout, ElementType type);

} // namespace vfc
