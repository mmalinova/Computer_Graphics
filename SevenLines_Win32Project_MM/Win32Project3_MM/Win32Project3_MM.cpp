// Win32Project3_MM.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Project3_MM.h"
#include "GdiPlus.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
int ak = 0;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
VOID DRAW_TEST(HDC hdc, int ak, int xend, int yend);

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

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32PROJECT3_MM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT3_MM));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	GdiplusShutdown(gdiplusToken);
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT3_MM));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32PROJECT3_MM);
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

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
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

	switch (message)
	{
	case WM_RBUTTONDOWN:
	{
					   InvalidateRect(hWnd, NULL, false);
					   ak++; if (ak > 7) ak = 0;
					   ::UpdateWindow(hWnd);
	}
		break;
		
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
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		RECT rt;
		GetClientRect(hWnd, &rt);
		DRAW_TEST(hdc, ak, int(rt.right), int(rt.bottom));
		EndPaint(hWnd, &ps);
		//InvalidateRect(hWnd, NULL, true);
		//UpdateWindow(hWnd);
		break;
	case WM_SIZE:
		ak = 0;
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

VOID DRAW_TEST(HDC hdc, int ak, int xend, int yend)
{
	Graphics graphics(hdc); // инициация на графичен обект върху екрана
	Rect circleRect(xend / 3.44, yend / 6, xend / 3.5, yend / 1.8);  //дефиниране на кръг с горен ляв ъгъл 
	//на описващия правоъгълник в средата на растера и рамери ¼ растер
	Pen redPen(Color(255, 255, 0, 0)); // дефиниране на червен молив
	Pen orangePen(Color(255, 255, 165, 0)); // дефиниране на оранжев молив
	Pen yellowPen(Color(255, 255, 255, 0)); // дефиниране на жълт молив
	Pen greenPen(Color(255, 0, 128, 0)); // дефиниране на зелен молив
	Pen bluePen(Color(255, 0, 0, 255)); // дефиниране на син молив
	Pen purpelPen(Color(255, 75, 0, 30)); // дефиниране на лилав молив
	Pen pinkPen(Color(255, 238, 130, 238)); // дефиниране на розов молив
	if (ak == 0)  graphics.DrawEllipse(&bluePen, circleRect); //чертае елипса
	else {
		switch (ak) {
		case 1:
			graphics.DrawLine(&redPen, xend / 2.8, yend / 1.6, xend / 2.8, yend / 3); //чертае отсечка
			break;
		case 2:
			graphics.DrawLine(&orangePen, xend / 2.6, yend / 1.55, xend / 2.6, yend / 4); //чертае отсечка
			break;
		case 3:
			graphics.DrawLine(&yellowPen, xend / 2.43, yend / 1.5, xend / 2.43, yend / 5); //чертае отсечка
			break;
		case 4:
			graphics.DrawLine(&greenPen, xend / 2.28, yend / 1.4, xend / 2.28, yend / 6); //чертае отсечка
			break;
		case 5:
			graphics.DrawLine(&bluePen, xend / 2.15, yend / 1.5, xend / 2.15, yend / 5); //чертае отсечка
			break;
		case 6:
			graphics.DrawLine(&purpelPen, xend / 2.04, yend / 1.55, xend / 2.04, yend / 4); //чертае отсечка
			break;
		case 7:
			graphics.DrawLine(&pinkPen, xend / 1.95, yend / 1.6, xend / 1.95, yend / 3); //чертае отсечка
			break;
		}
	}
}