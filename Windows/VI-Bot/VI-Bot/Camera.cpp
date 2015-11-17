#include "stdafx.h"
#include "VI-Bot.h"

using namespace cv;

CLEyeCameraInstance StartCam(int FrameRate, CLEyeCameraResolution Resolution)
{
	CLEyeCameraInstance EyeCamera;

	//Check for presence of EYE
	int CameraCount = CLEyeGetCameraCount();
	if (CameraCount>0) std::cout << "Number of EYE's detected: " << CameraCount << std::endl;
	else{
		std::cout << "No camera detected, press any key...";
		getchar();
		assert(CameraCount>0);
		return 0;
	}

	//For each camera get its unique id and add to stack and initialize
	GUID CameraID;

	//get ID
	CameraID = CLEyeGetCameraUUID(0);
	//init camera
	EyeCamera = CLEyeCreateCamera(CameraID, CLEYE_COLOR_RAW, Resolution, FrameRate);
	assert(EyeCamera != NULL);
	//set camera paramas
	CLEyeSetCameraParameter(EyeCamera, CLEYE_AUTO_EXPOSURE, 1);
	CLEyeSetCameraParameter(EyeCamera, CLEYE_GAIN, 0);
	CLEyeSetCameraParameter(EyeCamera, CLEYE_AUTO_WHITEBALANCE, 1);


	//Start the eye camera
	CLEyeCameraStart(EyeCamera);

	return EyeCamera;
}

void StopCam(CLEyeCameraInstance & EyeCamera)
{
	CLEyeCameraStop(EyeCamera);
	CLEyeDestroyCamera(EyeCamera);
}
