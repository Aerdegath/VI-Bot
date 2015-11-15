// VI-Bot.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "VI-Bot.h"

#define MAX_LOADSTRING 100
#define COM_BASE 9000
#define COM_OPT1 COM_BASE+1
#define COM_OPT2 COM_BASE+2
#define COM_OPT3 COM_BASE+3
#define COM_OPT4 COM_BASE+4
#define COM_OPT5 COM_BASE+5
#define COM_OPT6 COM_BASE+6
#define COM_OPT7 COM_BASE+7

#define BTN_RUN  1001
#define BTN_LOAD 1002


#using <System.dll>

using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

ref class ManagedGlobals {
public:
	static array<String^>^ serialPorts = nullptr;          // COM port names
};

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

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

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

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
   msclr::interop::marshal_context mcontext;
   int count = 0;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 400, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   hMenu = GetMenu(hWnd);
   hMenu = GetSubMenu(hMenu, 1);
   hMenu = GetSubMenu(hMenu, 0);

   try
   {
	   // Get a list of serial port names.
	   ManagedGlobals::serialPorts = SerialPort::GetPortNames();
   }
   catch (Win32Exception^ ex)
   {
	   Console::WriteLine(ex->Message);
   }

   for each(String^ port in ManagedGlobals::serialPorts)
   {
	   AppendMenu(hMenu, MF_UNCHECKED, COM_OPT1+count, mcontext.marshal_as<LPCWSTR>(port));
	   count++;
   }

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
	   (HMENU)BTN_RUN,
	   GetModuleHandle(NULL),
	   NULL);

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
	MENUITEMINFO mInfo;
	HMENU hMenu = GetMenu(hWnd);
	String^ name;
	int serialIdx = 0;

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
