// VI-Bot.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "VI-Bot.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND listView;
vector<holeInfo> holeList;
int serialIdx = 0;
CLEyeCameraInstance eyeCam;
wchar_t **availablePorts = NULL;
int openPorts = 0;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INITCOMMONCONTROLSEX iccx;
	MSG msg;
	HACCEL hAccelTable;

	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&iccx);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_VIBOT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VIBOT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	for (int i = 0; i < openPorts; i++)
	{
		free(availablePorts[i]);
	}

	free(availablePorts);

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VIBOT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_VIBOT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   HMENU hMenu;
   LVCOLUMN lvc;
   WCHAR colText[256];

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1000, 500, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   hMenu = GetMenu(hWnd);
   hMenu = GetSubMenu(hMenu, 1);
   hMenu = GetSubMenu(hMenu, 0);

   loadCOMPorts();

   for (int i = 0; i < openPorts; i++)
   {
	   AppendMenu(hMenu, MF_UNCHECKED, COM_OPT1 + i, availablePorts[i]);
   }

   //select the first by default
   CheckMenuItem(hMenu, COM_OPT1, MF_UNCHECKED);

   //build controls
    CreateWindowEx(NULL,
	   L"BUTTON",
	   L"Start",
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   30,
	   120,
	   100,
	   24,
	   hWnd,
	   (HMENU)BTN_RUN,
	   GetModuleHandle(NULL),
	   NULL);

	CreateWindowEx(NULL,
	   L"BUTTON",
	   L"Load",
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   150,
	   120,
	   100,
	   24,
	   hWnd,
	   (HMENU)BTN_LOAD,
	   GetModuleHandle(NULL),
	   NULL);

	listView = CreateWindowEx(NULL,
		WC_LISTVIEW,
		L"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT | LVS_SINGLESEL| ES_AUTOVSCROLL,
		300,
		25,
		550,
		250,
		hWnd,
		(HMENU)CSV_LIST,
		GetModuleHandle(NULL),
		NULL);

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER | LVCFMT_FIXED_WIDTH;
	lvc.cx = 100;
	 
	for (int i = 0; i < CSV_COLS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = colText;
		
		if (i > 0)
			lvc.cx = 150;

		LoadString(hInst, IDS_CSVCOLS_1 + i, colText, sizeof(colText) / sizeof(colText[0]));

		ListView_InsertColumn(listView, i, &lvc);
	}

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HMENU hMenu = GetMenu(hWnd);
	OPENFILENAME fileStruct;
	wchar_t	filename[MAX_FILENAME_CHARS];

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case COM_OPT1:
		case COM_OPT2:
		case COM_OPT3:
		case COM_OPT4:
		case COM_OPT5:
		case COM_OPT6:
		case COM_OPT7:
			serialIdx = wmId - COM_BASE;
			CheckMenuItem(hMenu, COM_OPT1, MF_UNCHECKED);
			CheckMenuItem(hMenu, COM_OPT2, MF_UNCHECKED);
			CheckMenuItem(hMenu, COM_OPT3, MF_UNCHECKED);
			CheckMenuItem(hMenu, COM_OPT4, MF_UNCHECKED);
			CheckMenuItem(hMenu, COM_OPT5, MF_UNCHECKED);
			CheckMenuItem(hMenu, COM_OPT6, MF_UNCHECKED);
			CheckMenuItem(hMenu, COM_OPT7, MF_UNCHECKED);

			CheckMenuItem(hMenu, wmId, MF_CHECKED);
			break;
		case BTN_LOAD:
			memset(&(fileStruct), 0, sizeof(fileStruct));
			fileStruct.lStructSize = sizeof(fileStruct);
			fileStruct.lpstrFile = filename;
			filename[0] = 0;
			fileStruct.nMaxFile = MAX_FILENAME_CHARS;
			fileStruct.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
			fileStruct.lpstrFilter = L"CSV files\0*.csv\0All files\0*.*\0\0";

			if (!(GetOpenFileName(&fileStruct)) || filename[0] == '\0')
				break;		/* user cancelled load */

			if (loadCSV(filename, listView, holeList) != 0)
			{
				MessageBox(hWnd, L"Error loading selected .csv file.", L"File Error", MB_ICONERROR);
			}

			break;
		case BTN_RUN:
		{
			if (availablePorts == NULL)
			{
				loadCOMPorts();

				if (availablePorts == NULL)
				{
					MessageBox(hWnd, L"No serial connection to robot. Check your cables.", L"COM Error", MB_ICONERROR);
					break;
				}
			}

			int ret = traversePart(availablePorts[serialIdx], holeList);
		    
			if (ret == -1)
			{
				MessageBox(hWnd, L"Failed to open serial connection!", L"COM Error", MB_ICONERROR);
			}
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void loadCOMPorts()
{
	msclr::interop::marshal_context mcontext;
	int count = 0;
	array<String^>^ serialPorts = nullptr;
	LPCWSTR tempString;

	try
	{
		// Get a list of serial port names.
		serialPorts = SerialPort::GetPortNames();
	}
	catch (Win32Exception^ ex)
	{
		Console::WriteLine(ex->Message);
	}

	if (serialPorts->Length > 0)
	{
		availablePorts = (wchar_t **)malloc(serialPorts->Length*sizeof(wchar_t *));
		openPorts = serialPorts->Length;
	}

	for each(String^ port in serialPorts)
	{
		//append to menu and store in wchar buffer
		tempString = mcontext.marshal_as<LPCWSTR>(port);
		availablePorts[count] = (wchar_t *)malloc((wcslen(tempString) + 1)*sizeof(wchar_t));
		wcscpy(availablePorts[count], tempString);
		count++;
	}
}