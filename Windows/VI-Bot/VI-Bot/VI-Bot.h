#pragma once

#include "resource.h"
#include "stdafx.h"

#define MAX_FILENAME_CHARS 100
#define CSV_COLS 4

int loadCSV(LPWSTR filename, HWND listView, vector<double> &pointList);