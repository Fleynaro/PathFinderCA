#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <float.h>
#include <vector>
#include <map>
#include <queue>
#include <deque>
#include <math.h>
#include <bitset>
#include <list>
#include <map>
#include <mutex>


//----------------------------------------------------------
// Main Defines
//----------------------------------------------------------
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <process.h>
#elif defined(LINUX)
#include <pthread.h>
#include <dlfcn.h>
#define sscanf_s sscanf
#define sprintf_s sprintf
#define printf_s printf
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define SLEEP(x) { Sleep(x); }
#include <windows.h>
#elif defined(LINUX)
#include <unistd.h>
#define SLEEP(x) { usleep(x * 1000); }
#endif

#if defined(LINUX)
	#define alloca __builtin_alloca
#endif
#define NULL_POINT -1000.0f
#define PATHFINDER_VERSION "2.0 Road finding future supported."
#define MAX_PATHS_CREATED 40



typedef void(*logprintf_t)(char* format, ...);
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
extern logprintf_t logprintf;