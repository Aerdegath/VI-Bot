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

typedef struct detectedHole_t
{
	double diameter;
	double xPos;
	double yPos;
	int samples;
}detectedHole;

int loadCSV(LPWSTR filename, HWND listView, vector<holeInfo> &pointList);
CLEyeCameraInstance StartCam(int FrameRate, CLEyeCameraResolution Resolution);
void StopCam(CLEyeCameraInstance & EyeCamera);
int orientCamera(CLEyeCameraInstance eyeCamera, double *xPos, double *yPos, cv::Vec2d *heading);
vector<detectedHole> trackTraversal(CLEyeCameraInstance eyeCamera, HANDLE serial, double xPos, double yPos, cv::Vec2d heading);
int traversePart(wchar_t* tivaCOM, vector<holeInfo> holeList);
void loadCOMPorts();
void rotateVector(cv::Vec2d *vector, double theta);
int logHoles(vector<detectedHole> foundHoles, vector<holeInfo> holeList, double initialX, double initialY);
int holeExists(detectedHole hole, vector<detectedHole> holeList);