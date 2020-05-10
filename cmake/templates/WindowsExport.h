#pragma once
#ifdef VFC_BUILD
#define VFC_EXPORT __declspec(dllexport)
#else
#define VFC_EXPORT __declspec(dllimport)
#endif
