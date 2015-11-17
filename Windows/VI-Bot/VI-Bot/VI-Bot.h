#pragma once

#include "resource.h"
#include "stdafx.h"

#define MAX_FILENAME_CHARS 100
#define CSV_COLS 4

typedef struct holeInfo_t
{
	double diameter;
	double xPos;
	double yPos;
}holeInfo;

int loadCSV(LPWSTR filename, HWND listView, vector<holeInfo> &pointList);
CLEyeCameraInstance StartCam(int FrameRate, CLEyeCameraResolution Resolution);
void StopCam(CLEyeCameraInstance & EyeCamera);
int orientCamera(CLEyeCameraInstance eyeCamera);
int trackTraversal(CLEyeCameraInstance eyeCamera, HANDLE serial);
int traversePart(wchar_t* tivaCOM, vector<holeInfo> holeList);
void loadCOMPorts();