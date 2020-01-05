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

/**
 * @file
 * @brief Configuration macros for the project.
 */

#if defined(_WIN32)
#	define VFC_WINDOWS 1
#elif defined(linux)
#	define VFC_LINUX 1
#elif defined(__APPLE__)
#	define VFC_APPLE
#endif

#if defined(_MSC_VER)
#	define VFC_MSC 1
#elif defined(__clang__)
#	define VFC_CLANG 1
#elif defined(__GNUC__)
#	define VFC_GCC 1
#else
#error Unknown compiler.
#endif

/**
 * @brief Define for whether the platform is Windows.
 */
#ifndef VFC_WINDOWS
#	define VFC_WINDOWS 0
#endif

/**
 * @brief Define for whether the platform is Linux.
 */
#ifndef VFC_LINUX
#	define VFC_LINUX 0
#endif

/**
 * @brief Define for whether the platform is Apple.
 */
#ifndef VFC_APPLE
#	define VFC_APPLE 0
#endif

/**
 * @brief Define for whether the compler is Microsoft's C compiler.
 */
#ifndef VFC_MSC
#	define VFC_MSC 0
#endif

/**
 * @brief Define for whether the compiler is LLVM clang.
 */
#ifndef VFC_CLANG
#	define VFC_CLANG 0
#endif

/**
 * @def VFC_GCC
 * @brief Define for whether the compiler is GCC.
 */
#ifndef VFC_GCC
#	define VFC_GCC 0
#endif

/**
 * @brief Macro defined to whether or not the system is 64-bit.
 */
#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__arm64__)
#define VFC_64BIT 1
#else
#define VFC_64BIT 0
#endif

/**
 * @brief Define for whether or not this is a debug build.
 */
#ifdef NDEBUG
#define VFC_DEBUG 0
#else
#define VFC_DEBUG 1
#endif

/**
 * @brief Macro for an unused variable.
 *
 * This can be used to work around compiler warnings.
 * @param x The unused variable.
 */
#define VFC_UNUSED(x) (void)(&x)

#if VFC_MSC
#pragma warning(disable: 4251) // 'x' needs to have dll-interface to be used by clients of class 'y'
#endif
