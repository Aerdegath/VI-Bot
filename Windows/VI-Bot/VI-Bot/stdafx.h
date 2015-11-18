// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <msclr\marshal.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#define _USE_MATH_DEFINES

// TODO: reference additional headers your program requires here
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/ocl/ocl.hpp"
#include "opencv2/nonfree/ocl.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "CLEyeMultiCam.h"

#using <System.dll>

#pragma comment (lib, "comctl32.lib")
#include <commctrl.h>

using namespace std;
using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")