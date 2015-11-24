#include "stdafx.h"
#include "VI-Bot.h"

#define LOG_PATH "C:\\Users\\Scott\\Desktop\\VI-Bot.txt"

#define BAUD_RATE 38400
#define CALIBRATION_VALUE 0.1
#define CALIB_ITERATIONS 200
#define THRESH_X 1
#define THRESH_Y 1
#define MAX_X 15
#define MAX_Y 15
#define MAX_DIAM 10

//conversion for affine translations (compensation for scaling error, root cause unknown)
//#define AFFINE_PIX2MM(X) ((double) X * (100.0/258.0))
#define AFFINE_PIX2MM(X) ((double)X * (100.0 / 315.0))
//conversion for objects in image
#define PIX2MM(X) ((double) X * 0.1078125)
//fudge factor for angle also try 2.87846481
//#define ANGLEERR(X) (X*0.001111133)
#define ANGLEERR(X) (X*0)

using namespace cv;

int traversePart(wchar_t* tivaCOM, vector<holeInfo> holeList)
{
	CLEyeCameraInstance eyeCamera;
	HANDLE serial = CreateFile(tivaCOM, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	DCB commConfig;
	COMMTIMEOUTS commTO;
	double xPos, yPos;
	Vec2d heading;
	vector<detectedHole> foundHoles;
	int errorsFound = 0;


	if (GetCommState(serial, &commConfig) == 0)
		return -1;

	if (commConfig.BaudRate != BAUD_RATE)
	{
		commConfig.BaudRate = BAUD_RATE;
		
		if (SetCommState(serial, &commConfig) == 0)
			return -1;
	}

	commTO.ReadIntervalTimeout = 3;
	commTO.ReadTotalTimeoutMultiplier = 3;
	commTO.ReadTotalTimeoutConstant = 2;
	commTO.WriteTotalTimeoutMultiplier = 3;
	commTO.WriteTotalTimeoutConstant = 2;
	SetCommTimeouts(serial, &commTO);

	eyeCamera = StartCam(60, CLEYE_VGA);
	
	orientCamera(eyeCamera, &xPos, &yPos, &heading);

	foundHoles = trackTraversal(eyeCamera, serial, xPos, yPos, heading);

//	errorsFound = logHoles(foundHoles, holeList, xPos, yPos);

	CloseHandle(serial);
	StopCam(eyeCamera);

	return 0;
}

bool minMatch(DMatch a, DMatch b) { return (a.distance < b.distance); }


//problem with taking the average, causes the center point to drift?
int orientCamera(CLEyeCameraInstance eyeCamera, double *xPos, double *yPos, Vec2d *heading)
{
	Mat frame, frameGray;
	vector<Vec3f> circles;
	float error = 2.0;
	int Height, Width;
	bool done = false;
	double angle = 0, avgAngle = 0;
	char tempBuffer[50];
	Vec2f avg1, avg2;

	CLEyeCameraGetFrameDimensions(eyeCamera, Width, Height);
	frame = Mat(Height, Width, CV_8UC4);

	while (waitKey(20) == -1)
	{
		//Get Frame From Camera
		CLEyeCameraGetFrame(eyeCamera, frame.data);
		imshow("Video Capture", frame);
	}

	destroyWindow("Video Capture");
	
	for (int i = 0; i < CALIB_ITERATIONS; i++)
	{
		CLEyeCameraGetFrame(eyeCamera, frame.data);
		cvtColor(frame, frameGray, CV_BGR2GRAY);

		medianBlur(frameGray, frameGray, 5);
		GaussianBlur(frameGray, frameGray, Size(3, 3), 3, 3);
		HoughCircles(frameGray, circles, CV_HOUGH_GRADIENT, 1, frameGray.rows / 8, 100, 25, 10, 140);

		if (circles.size() < 2)
		{
			i--;
			continue;
		}

		if (circles[0][0] > circles[1][0])
		{
			avg1[0] += circles[0][0];
			avg1[1] += circles[0][1];
			avg2[0] += circles[1][0];
			avg2[1] += circles[1][1];
		}
		else
		{
			avg2[0] += circles[0][0];
			avg2[1] += circles[0][1];
			avg1[0] += circles[1][0];
			avg1[1] += circles[1][1];
		}


	}

	avg1[0] = avg1[0] / CALIB_ITERATIONS;
	avg1[1] = avg1[1] / CALIB_ITERATIONS;
	avg2[0] = avg2[0] / CALIB_ITERATIONS;
	avg2[1] = avg2[1] / CALIB_ITERATIONS;

	Point circle1(cvRound(avg1[0]), cvRound(avg1[1]));
	Point circle2(cvRound(avg2[0]), cvRound(avg2[1]));

	//calculate angles and real-world position of image origin (convieniently equivalent to center point of board origin)
	if (circle1.x > circle2.x)
	{
		angle = (atan2(circle1.y - circle2.y, circle1.x - circle2.x));
		*xPos = PIX2MM(circle1.y);
		*yPos = PIX2MM(circle1.x);
	}
	else
	{
		angle = (atan2(circle2.y - circle1.y, circle2.x - circle1.x));
		*xPos = PIX2MM(circle2.y);
		*yPos = PIX2MM(circle2.x);
	}

	circle(frame, circle2, 1, Scalar(0, 0, 255), 2);
	circle(frame, circle1, 1, Scalar(0, 0, 255), 2);

	(*heading)[0] = cos(angle);
	(*heading)[1] = sin(angle);
	
	sprintf(tempBuffer, "%.4lf x: %.4lf y: %.4lf", (angle * 180) / M_PI, (*heading)[0], (*heading)[1]);

	putText(frame, tempBuffer, Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0, 255, 0));

	imshow("Calibration", frame);
	waitKey(0);

	destroyWindow("Calibration");

	return 0;
}

vector<detectedHole> trackTraversal(CLEyeCameraInstance eyeCamera, HANDLE serial, double xPos, double yPos, Vec2d xheading)
{
	int Width, Height;
	int minHessian = 13000;
	double distX = 0, distY = 0, deltaX = 0, deltaY = 0;
	vector<Point2f> affinePts[2];
	Mat framePrev, framePrevGray, frame, frameGray, frameWarp;
	Ptr<CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(500);

	ocl::SURF_OCL ocl_surf(minHessian);
	ocl::BFMatcher_OCL matcher(NORM_L2);
	std::vector<KeyPoint> keypointsPrev, keypointsFrame;
	ocl::oclMat descriptorsPrev, descriptorsFrame;
	std::vector< DMatch > matches;
	Size gaussSize;
	Mat affine;
	ocl::oclMat img1, img2;
	int nullCount = 0;
    char serialBuffer[50];
	char printBuffer[200];
	DWORD length;
    DWORD lpErr;
	COMSTAT lpStat;
	std::vector<Vec3f> circles;
	double vectorNorm;
	double theta;
	Vec2d yheading;
	vector<detectedHole> foundHoles;
	detectedHole thisHole;
	int foundIdx = 0;
	double initialX = xPos;
	double initialY = yPos;

	gaussSize.width = 3;
	gaussSize.height = 3;

	CLEyeCameraGetFrameDimensions(eyeCamera, Width, Height);

	frame = Mat(Height, Width, CV_8UC4);
	framePrev = Mat(Height, Width, CV_8UC4);

	//initialize the camera
	for (int i = 0; i < 50; i++)
	{
		CLEyeCameraGetFrame(eyeCamera, frame.data);
		imshow("init", frame);
		waitKey(1);
	}

	destroyWindow("init");

	while (1)
	{
		CLEyeCameraGetFrame(eyeCamera, framePrev.data);
		cvtColor(framePrev, framePrevGray, CV_BGR2GRAY);
		medianBlur(framePrevGray, framePrevGray, 5);
		GaussianBlur(framePrevGray, framePrevGray, gaussSize, gaussSize.width, gaussSize.height);
		clahe->apply(framePrevGray, framePrevGray);

		img1 = framePrevGray;

		ocl_surf(img1, ocl::oclMat(), keypointsPrev, descriptorsPrev);

		CLEyeCameraGetFrame(eyeCamera, frame.data);
		cvtColor(frame, frameGray, CV_BGR2GRAY);
		medianBlur(frameGray, frameGray, 5);
		GaussianBlur(frameGray, frameGray, gaussSize, gaussSize.width, gaussSize.height);
		HoughCircles(frameGray, circles, CV_HOUGH_GRADIENT, 1, frameGray.rows / 8, 100, 35, 0, 0);
		clahe->apply(frameGray, frameGray);
		
		img2 = frameGray;

		ocl_surf(img2, ocl::oclMat(), keypointsFrame, descriptorsFrame);

		matcher.match(descriptorsPrev, descriptorsFrame, matches);

		std::sort(matches.begin(), matches.end(), minMatch);
		
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);

			// circle center
			circle(frameGray, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			// circle outline
			circle(frameGray, center, radius, Scalar(0, 0, 255), 3, 8, 0);

			sprintf(printBuffer, "X: %.2lf Y: %.2lf Size: %.2lf", xPos - PIX2MM(center.y), yPos - PIX2MM(center.x), PIX2MM(radius));
			putText(frame, printBuffer, Point(center.x - 30, center.y + radius + 5), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 0));

			thisHole.xPos = xPos - PIX2MM(center.y);
			thisHole.yPos = yPos - PIX2MM(center.x);
			thisHole.diameter = PIX2MM(radius * 2);
			thisHole.samples = 1;

			foundHoles.push_back(thisHole);
		}

		for (int i = 0; i < floor(matches.size()*0.20); i++)
		{
			affinePts[0].push_back(keypointsPrev[matches[i].queryIdx].pt);
			affinePts[1].push_back(keypointsFrame[matches[i].trainIdx].pt);
		}

		if (affinePts[0].size() < 3)
		{
			continue;
		}

		affine = estimateRigidTransform(affinePts[0], affinePts[1], true);

		if (affine.empty())
		{
			//try again
			nullCount++;

			vector<Point2f>().swap(affinePts[0]);
			vector<Point2f>().swap(affinePts[1]);

			continue;
		}

		//multiply the heading vector by the affine rotation matrix
		theta = ANGLEERR((atan2(affine.at<double>(1, 0), affine.at<double>(0, 0))));
		rotateVector(&xheading, theta);

		vectorNorm = sqrt(pow(xheading[0], 2) + pow(xheading[1], 2));

		xheading[0] /= vectorNorm;

		yheading[0] = xheading[0];
		yheading[1] = xheading[1];
		rotateVector(&yheading, M_PI/2);

		if (abs(affine.at<double>(0, 2)) > THRESH_X)
		{
			distX += affine.at<double>(0, 2);
			deltaX = affine.at<double>(0, 2);
		}

		if (abs(affine.at<double>(1, 2)) > THRESH_Y)
		{
			distY += affine.at<double>(1, 2);
			deltaY = affine.at<double>(1, 2);
		}

		//deltaY/X is delta with respect to the image; flipped in real-world system
		xPos += AFFINE_PIX2MM(deltaY) * xheading[0] + AFFINE_PIX2MM(deltaX) * yheading[0];
		yPos += AFFINE_PIX2MM(deltaX) * yheading[1] + AFFINE_PIX2MM(deltaY) * xheading[1];

		sprintf(serialBuffer, "%.2lf,%.2lf\n", deltaX, deltaY);

		sprintf(printBuffer, "%.2lf, %.2lf pos1: %.2lf, %.2lf", 
			deltaX, deltaY, xPos, yPos);
		deltaX = deltaY = 0;
		putText(frame, printBuffer, cvPoint(0, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(80, 255, 80));
		
		if (WriteFile(serial, serialBuffer, strlen(serialBuffer), &length, NULL) == 0)
		{
			OutputDebugString(L"Failed to write to serial.");
		}

		imshow("Video Feed", frame);

		vector<Point2f>().swap(affinePts[0]);
		vector<Point2f>().swap(affinePts[1]);

		ClearCommError(serial, &lpErr, &lpStat);
		
		if (lpStat.cbInQue > 0)
		{
			if (ReadFile(serial, serialBuffer, 5, &length, NULL) != 0)
			{
				serialBuffer[5] = '\0';
				if (strcmp("done\n", serialBuffer) == 0)
				{
					//discovered we're finished
					break;
				}
			}
		}

		waitKey(1);
	}

	destroyWindow("Video Feed");

	return foundHoles;
}

void rotateVector(Vec2d *vector, double theta)
{
	double newX, newY;
	newX = (*vector)[0] * cos(theta) - (*vector)[1] * sin(theta);
	newY = (*vector)[0] * sin(theta) + (*vector)[1] * cos(theta);

	(*vector)[0] = newX;
	(*vector)[1] = newY;
}

//this is the WORST way to do this
//search holeList for a hole within range of the current one, return index
int holeExists(detectedHole hole, vector<detectedHole> holeList)
{
	for (int i = 0; i < holeList.size(); i++)
	{
		if ((abs(hole.xPos - holeList[i].xPos) < MAX_X) &&
			(abs(hole.yPos - holeList[i].yPos) < MAX_Y) &&
			abs(hole.diameter - holeList[i].diameter) < MAX_DIAM)
		{
			return i;
		}
	}

	return -1;
}

//again no time do the stupid way
int logHoles(vector<detectedHole> foundHoles, vector<holeInfo> holeList, double initialX, double initialY)
{
	int i, j;
	FILE *logFile;
	int foundIdx;

	logFile = fopen(LOG_PATH, "w");

	//consolidate holes
	for (i = 0; i < foundHoles.size(); i++)
	{
		//skip the fiducial markers
		if (foundHoles[i].xPos > initialX || foundHoles[i].yPos > initialY)
		{
			foundHoles.erase(foundHoles.begin() + i);
		}

	    foundIdx = holeExists(foundHoles[i], foundHoles);

	    if (foundIdx != -1)
	    {
			//combine the two
			foundHoles[foundIdx].samples++;
			foundHoles[foundIdx].diameter += foundHoles[i].diameter;
			foundHoles[foundIdx].diameter /= foundHoles[foundIdx].samples;
			foundHoles[foundIdx].xPos += foundHoles[i].xPos;
			foundHoles[foundIdx].xPos /= foundHoles[foundIdx].samples;
			foundHoles[foundIdx].yPos += foundHoles[i].yPos;
			foundHoles[foundIdx].yPos /= foundHoles[foundIdx].samples;

			foundHoles.erase(foundHoles.begin() + i);
		}
	}
	

	fprintf(logFile, "Detected Holes (all units mm):\n\n");

	for (i = 0; i < foundHoles.size(); i++)
	{
		fprintf(logFile, "%d\tX: %.3lf\tY: %.3lf\tDiameter: %.3lf\n", i + 1,
			     foundHoles[i].xPos, foundHoles[i].yPos, foundHoles[i].diameter);

	}

	for (i = 0; i < foundHoles.size(); i++)
	{
		for (j = 0; j < holeList.size(); j++)
		{
			if ((abs(foundHoles[i].xPos - holeList[j].xPos) < MAX_X) &&
				(abs(foundHoles[i].yPos - holeList[j].yPos) < MAX_Y) &&
				abs(foundHoles[i].diameter - holeList[j].diameter) < MAX_DIAM)
			{
				//we have a match
				foundHoles.erase(foundHoles.begin()+i);
				break;
			}
		}
	}

	if (foundHoles.size() > 0)
	{
		fprintf(logFile, "\n\nThe following errors were detected:\n\n");
		//only thing left don't have matches
		for (i = 0; i < foundHoles.size(); i++)
		{
			fprintf(logFile, "%d\tX: %.3lf\tY: %.3lf\tDiameter: %.3lf\n", i + 1,
				foundHoles[i].xPos, foundHoles[i].yPos, foundHoles[i].diameter);

		}

		fclose(logFile);

		return 1;
	}

	fclose(logFile);

	return 0;

}