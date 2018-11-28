#pragma once

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define FSP_CALL __cdecl
#if defined(FSP_EXPORT)
#define FSP_API extern "C" __declspec(dllexport)
#else
#define FSP_API extern "C" __declspec(dllimport)
#endif
#elif defined(__APPLE__)
#define FSP_API __attribute__((visibility("default"))) extern "C"
#define FSP_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#define FSP_API extern "C" __attribute__((visibility("default")))
#define FSP_CALL
#else
#define FSP_API extern "C"
#define FSP_CALL
#endif

#include <assert.h>

#define	FSP_MININUM_STRING_CAPACITY	32

FSP_API void* FSP_CALL	FspMalloc(size_t size);
FSP_API void  FSP_CALL	FspFree(void* block);
FSP_API void  FSP_CALL	FspMemCpy(void* dst, const void* src, int n);
FSP_API void  FSP_CALL	FspMemMov(void* dst, const void* src, int n);
FSP_API int	  FSP_CALL	FspStrLen(const char* s);
FSP_API int	  FSP_CALL	FspStrCmp(const char* lhs, const char* rhs);