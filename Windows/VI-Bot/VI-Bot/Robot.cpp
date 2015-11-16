#include "stdafx.h"
#include "VI-Bot.h"

#define BAUD_RATE 9600
#define CALIBRATION_VALUE 0.1
#define THRESH_X 1
#define THRESH_Y 1

using namespace cv;

int traversePart(System::String^ tivaCOM, vector<holeInfo> holeList)
{
	CLEyeCameraInstance eyeCamera;
	SerialPort ^ serial = gcnew SerialPort(tivaCOM, BAUD_RATE);
	serial->ReadTimeout = 150;

	serial->Open();

	if (!serial->IsOpen)
		return -1;

	eyeCamera = StartCam(60, CLEYE_VGA);

	//orientCamera(eyeCamera);

	trackTraversal(eyeCamera, serial);

	return 0;
}

bool minMatch(DMatch a, DMatch b) { return (a.distance < b.distance); }

int orientCamera(CLEyeCameraInstance eyeCamera)
{
	Mat frame, frameGray;
	vector<Vec3f> circles;
	float error = 2.0;
	int Height, Width;

	CLEyeCameraGetFrameDimensions(eyeCamera, Width, Height);
	frame = Mat(Height, Width, CV_8UC4);

	while (error > CALIBRATION_VALUE)
	{
		CLEyeCameraGetFrame(eyeCamera, frame.data);
		cvtColor(frame, frameGray, CV_BGR2GRAY);

		GaussianBlur(frameGray, frameGray, Size(5, 5), 5, 5);
		HoughCircles(frameGray, circles, CV_HOUGH_GRADIENT, 1, frameGray.rows / 8, 100, 25, 0, 0);

		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);

			// circle center
			circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			// circle outline
			circle(frame, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		}

		if (circles.size() < 2)
		{
			continue;
		}

		Point circle1(cvRound(circles[0][0]), cvRound(circles[0][1]));
		Point circle2(cvRound(circles[1][0]), cvRound(circles[1][1]));

		error = circle1.y - circle2.y;

		if (error > CALIBRATION_VALUE)
		{
			line(frame, circle1, circle2, Scalar(0, 0, 255), 4);
		}
		else
		{
			line(frame, circle1, circle2, Scalar(0, 255, 0), 4);
		}

		imshow("Calibration", frame);
		waitKey(100);
	}

	return 0;
}

int trackTraversal(CLEyeCameraInstance eyeCamera, SerialPort^ serial)
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
	System::String^ out;
    char tempBuffer[50];

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
		clahe->apply(frameGray, frameGray);
		
		img2 = frameGray;

		ocl_surf(img2, ocl::oclMat(), keypointsFrame, descriptorsFrame);

		matcher.match(descriptorsPrev, descriptorsFrame, matches);

		std::sort(matches.begin(), matches.end(), minMatch);

		for (int i = 0; i < floor(matches.size()*0.20); i++)
		{
			affinePts[0].push_back(keypointsPrev[matches[i].queryIdx].pt);
			affinePts[1].push_back(keypointsFrame[matches[i].trainIdx].pt);
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

		sprintf(tempBuffer, "%.2lf,%.2lf\n", deltaX, deltaY);
		out = gcnew System::String(tempBuffer);
		serial->Write(out);

		imshow("feed gray", frameGray);

		vector<Point2f>().swap(affinePts[0]);
		vector<Point2f>().swap(affinePts[1]);

		if (serial->BytesToRead > 0)
		{
			try
			{
			   out = serial->ReadLine();
			}
			catch (System::Exception ^e)
			{
				//meh
			}
		}

		if (out->Contains("done"))
		{
			break;
		}

		waitKey(1);
	}

	return 0;
}