// Daisy_Image_Win32Project_MM.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Daisy_Image_Win32Project_MM.h"
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
INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
VOID DRAW_Daisy(HDC hdc, int ak, int xend, int yend);
//VOID Example_SaveFile(HDC hdc);
//VOID Example_BMPbuffer(HDC hdc, int ak, int xend, int yend);

INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid)  // helper function
{//помощна функция за извличане на параметри на енкодера
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}


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
	LoadString(hInstance, IDC_DAISY_IMAGE_WIN32PROJECT_MM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DAISY_IMAGE_WIN32PROJECT_MM));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DAISY_IMAGE_WIN32PROJECT_MM));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DAISY_IMAGE_WIN32PROJECT_MM);
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

   HBRUSH brush = CreateSolidBrush(0x00FF0000);
   SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);

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

int TIMER = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	SetTimer(hWnd,
		TIMER,
		300,
		(TIMERPROC)NULL);

	switch (message)
	{
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, false);
		//ak++;
		if (ak > 15) {
			InvalidateRect(hWnd, NULL, true);
			ak = 0;
		}
		::UpdateWindow(hWnd);
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
		DRAW_Daisy(hdc, ak, int(rt.right), int(rt.bottom));
		EndPaint(hWnd, &ps);
		SetTimer(hWnd, TIMER, 400, (TIMERPROC)NULL);
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

VOID DRAW_Daisy(HDC hdc, int ak, int xend, int yend)
{
	// ak – помощен брояч
	// xend, yend – размер на растера

	Graphics graphics(hdc);  // инициализация на графичен режим
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	Bitmap bmp(xend, yend, &graphics); //създаване на обект - буфер
	Graphics imgr(&bmp); // създаване на обект от буфера
	imgr.Clear(Color(0, 50, 255)); //задаване на базов цвят бял екран
	Rect circleRect(xend / 2.2, yend / 4, xend / 18, yend / 10);  //дефиниране на кръг с горен ляв ъгъл 
	//на описващия правоъгълник в средата на растера и рамери ¼ растер

	PointF center(xend / 2.2 + xend / 36, yend / 4 + yend / 20);

	Pen blackPen(Color(255, 0, 0, 0)); // дефиниране на жълт молив

	SolidBrush yellowBrush = Color(255, 255, 255, 0);

	SolidBrush whiteBrush = Color(255, 255, 255, 255);

	Pen greenPen(Color(255, 119, 255, 104)); // дефиниране на светло зелен молив

	int dx = xend / 9;			       // определяне на стъпка по х	
	int dy = yend / 2;			       // определяне на стъпка по y

	Point p[] = {			// листа
		Point(xend / 2.33 + xend / 36, yend / 4 + yend / 20),
		Point(xend / 3, yend / 2),
		Point(xend / 3, yend / 5),
		Point(xend / 2.33 + xend / 36, yend / 4 + yend / 20),
	};

	Point p1[] = {		// градинка
		Point(0, dy + 10),
		Point(dx, dy + 20),
		Point(2 * dx, dy - 20),
		Point(3 * dx, dy + 10),
		Point(4 * dx, dy + 20),
		Point(5 * dx, dy - 20),
		Point(6 * dx, dy + 10),
		Point(7 * dx, dy + 20),
		Point(8 * dx, dy - 20),
		Point(xend, dy + 10)
	};

	Pen Gardenpen(Color(255, 0, 128, 0));
	SolidBrush Gardenbr(Color(255, 0, 128, 0));

	GraphicsPath myPath, myPath1;
	Matrix myMatrix; //иницииране на матрица за трансформации

	myPath1.StartFigure();

	for (ak = 0; ak <= 14; ak++) {
		if (ak == 0)
		{
			myPath.AddLine(0, yend, 0, dy + 5);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddBeziers(p1, 10);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddLine(xend, dy + 5, xend, yend);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);

			myPath.AddLine(0, yend, 0, dy + 10);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddBeziers(p1, 10);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddLine(xend, dy + 10, xend, yend);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);

			myPath.AddLine(0, yend, 0, dy + 5);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddBeziers(p1, 10);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddLine(xend, dy + 5, xend, yend);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);

			myPath.AddLine(0, yend, 0, dy);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddBeziers(p1, 10);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddLine(xend, dy, xend, yend);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);

			myPath.AddLine(0, yend, 0, dy + 10);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddBeziers(p1, 10);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
			myPath.AddLine(xend, dy + 10, xend, yend);
			imgr.DrawPath(&Gardenpen, &myPath);
			imgr.FillPath(&Gardenbr, &myPath);
		}

		switch (ak) {
		case 1:
		{
				  imgr.DrawBezier(&greenPen, xend / 2.07, yend / 2.85, xend / 2, yend / 2.4, xend / 2.3, yend / 2, xend / 2.1, yend / 1.3);
		}
			break;
		case 2:
		{
				  imgr.DrawEllipse(&blackPen, circleRect);
				  imgr.FillEllipse(&yellowBrush, circleRect);
		}
			break;
		case 3:
		{
				  myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				  myMatrix.RotateAt(0, center);
				  myPath1.Transform(&myMatrix);
		}
			break;
		case 4:
		{
				  myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				  myMatrix.RotateAt(-30, center);
				  myPath1.Transform(&myMatrix);
		}
			break;
		case 5:
		{
				  myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				  myMatrix.RotateAt(-60, center);
				  myPath1.Transform(&myMatrix);
		}
			break;
		case 6:
		{
				  myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				  myMatrix.RotateAt(-90, center);
				  myPath1.Transform(&myMatrix);
		}
			break;
		case 7:
		{
				  myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				  myMatrix.RotateAt(-120, center);
				  myPath1.Transform(&myMatrix);
		}
			break;
		case 8:
		{
				  myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				  myMatrix.RotateAt(-150, center);
				  myPath1.Transform(&myMatrix);
		}
			break;
		case 9:
		{
				  myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				  myMatrix.RotateAt(-180, center);
				  myPath1.Transform(&myMatrix);
		}
			break;
		case 10:
		{
				   myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				   myMatrix.RotateAt(-210, center);
				   myPath1.Transform(&myMatrix);
		}
			break;
		case 11:
		{
				   myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				   myMatrix.RotateAt(-240, center);
				   myPath1.Transform(&myMatrix);
		}
			break;
		case 12:
		{
				   myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				   myMatrix.RotateAt(-270, center);
				   myPath1.Transform(&myMatrix);
		}
			break;
		case 13:
		{
				   myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				   myMatrix.RotateAt(-300, center);
				   myPath1.Transform(&myMatrix);
		}
			break;
		case 14:
		{
				   myPath1.AddBezier(p[0], p[1], p[2], p[3]);
				   myMatrix.RotateAt(-330, center);
				   myPath1.Transform(&myMatrix);
		}
			break;
		}

		imgr.DrawPath(&blackPen, &myPath1);
		imgr.FillPath(&whiteBrush, &myPath1);
		
		myPath1.CloseFigure();
		myPath.CloseFigure();
	}

	graphics.DrawImage(&bmp, 0, 0); // визуализация на буфера върху екрана

	//промени в параметрите на енкодера 
	CLSID clsid;
	EncoderParameters encoderParameters;
	ULONG             quality;

	GetEncoderClsid(L"image/jpeg", &clsid);
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;

	// запис на JPEG формат изображение с компресия 99.
	quality = 99;
	encoderParameters.Parameter[0].Value = &quality;

	bmp.Save(L"Daisy_MM.bmp", &clsid, &encoderParameters);
}
