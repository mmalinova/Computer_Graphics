// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here

#include <gdiplus.h>

using namespace Gdiplus;

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#pragma comment(lib, "GdiPlus.lib")