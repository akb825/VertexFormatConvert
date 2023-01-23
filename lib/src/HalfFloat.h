/*
 * Copyright 2023 Aaron Barany
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

#include <cassert>
#include <cstdint>

#if VFC_X86_32 || VFC_X86_64
#include <immintrin.h>
#define VFC_SSE 1
#define VFC_NEON 0
#if defined(__F16C__)
#define VFC_ALWAYS_HARDWARE_HALF_FLOAT 1
#else
#define VFC_ALWAYS_HARDWARE_HALF_FLOAT 0
#endif
#elif VFC_ARM_32 || VFC_ARM_64
#include <arm_neon.h>
#define VFC_SSE 0
#define VFC_NEON 1
#define VFC_ALWAYS_HARDWARE_HALF_FLOAT 1
#else
#define VFC_SSE 0
#define VFC_NEON 0
#define VFC_ALWAYS_HARDWARE_HALF_FLOAT 0
#endif

#if VFC_SSE && VFC_CLANG
#define VFC_START_HALF_FLOAT() \
	_Pragma("clang attribute push(__attribute__((target(\"sse,sse2,f16c\"))), apply_to = function)")
#define VFC_END_HALF_FLOAT() _Pragma("clang attribute pop")
#elif VFC_SSE && VFC_GCC
#define VFC_START_HALF_FLOAT() \
	_Pragma("GCC push_options") \
	_Pragma("GCC target(\"sse,sse2,f16c\")")
#define VFC_END_HALF_FLOAT() _Pragma("GCC pop_options")
#else
#define VFC_START_HALF_FLOAT()
#define VFC_END_HALF_FLOAT()
#endif

#if VFC_GCC || VFC_CLANG
#	define VFC_ALIGN(x) __attribute__((aligned(x)))
#elif VFC_MSC
#	define VFC_ALIGN(x) __declspec(align(x))
#else
#error Need to provide alignment implementation for this compiler.
#endif

namespace vfc
{

// Export for unit tests.
extern const bool hasHardwareHalfFloat;

VFC_START_HALF_FLOAT()

inline void packHardwareHalfFloat4(std::uint16_t result[4], const float value[4])
{
#if VFC_SSE
	__m128 f = _mm_load_ps(value);
	__m128i h = _mm_cvtps_ph(f, 0);
	_mm_storeu_si64(result, h);
#elif VFC_NEON
	float32x4_t f = vld1q_f32(value);
	float16x4_t h = vcvt_f16_f32(f);
	vst1_f16(reinterpret_cast<float16_t*>(result), h);
#else
	VFC_UNUSED(result);
	VFC_UNUSED(value);
	assert(false);
#endif
}

inline void unpackHardwareHalfFloat4(float result[4], const std::uint16_t value[4])
{
#if VFC_SSE
	__m128i h = _mm_loadu_si64(value);
	__m128 f = _mm_cvtph_ps(h);
	_mm_store_ps(result, f);
#elif VFC_NEON
	float16x4_t h = vld1_f16(reinterpret_cast<const float16_t*>(value));
	float32x4_t f = vcvt_f32_f16(h);
	vst1q_f32(result, f);
#else
	VFC_UNUSED(result);
	VFC_UNUSED(value);
	assert(false);
#endif
}

VFC_END_HALF_FLOAT()

} // namespace cuttlefish
