#pragma once

#if !defined UNICODE || !defined _UNICODE
#error "Unicode must be defined"
#endif

#include <winsdkver.h>

// Targeting Windows 10 and later
#define WINVER 0x0A00 
#define _WIN32_WINNT 0x0A00

#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
