#include "stdafx.h"
#include "VI-Bot.h"

using namespace cv;
using namespace cv::ocl;

int loadCSV(LPWSTR filename, HWND listView, vector<double> &pointList)
{
	wifstream csvFile;
	LVITEM nextItem = { 0 };
	int fileIdx = 0, rowIdx = 0;
	wchar_t temp[256];

	csvFile.open(filename);

	if (!csvFile.is_open())
	{
		OutputDebugString(L"Couldn't open CSV file");
		return -1;
	}

	//read first two lines
	csvFile.getline(temp, 256, L'\n');
	csvFile.getline(temp, 256, L'\n');

	while (csvFile.good())
	{
		//last item delimited by newline
		if ((fileIdx % CSV_COLS) == CSV_COLS - 1)
		{
			csvFile.getline(temp, 256, L'\n');
		}
		else
		{
			csvFile.getline(temp, 256, L',');
		}

		if (temp[0] == '\0')
			break;

		nextItem.iItem = rowIdx;
		
		if (!(fileIdx % CSV_COLS))
		{
			ListView_InsertItem(listView, &nextItem);
		}

		ListView_SetItemText(listView, rowIdx, (fileIdx % CSV_COLS), temp);

		if ((fileIdx % CSV_COLS) != 0)
		{
			pointList.push_back(wcstod(temp, NULL));
		}

		if ((fileIdx % CSV_COLS) == CSV_COLS - 1)
		{
			rowIdx++;
		}
			

		fileIdx++;
		
	}

	return 0;

}