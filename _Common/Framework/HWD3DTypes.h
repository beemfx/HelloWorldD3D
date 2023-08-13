// D3D Hello World

#pragma once

#include "HWD3DMatrix.h"

#include "targetver.h"
#include "Resource.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <vector>
#include <string>
#include <guiddef.h>
#include <cassert>

static const float HWD3D_PI_CONST = 3.141592654f;

static float HWD3D_ToRad(float d) { return ((d) * (HWD3D_PI_CONST / 180.0f)); }
static float HWD3D_ToDeg(float r) { return ((r) * (180.0f / HWD3D_PI_CONST)); }