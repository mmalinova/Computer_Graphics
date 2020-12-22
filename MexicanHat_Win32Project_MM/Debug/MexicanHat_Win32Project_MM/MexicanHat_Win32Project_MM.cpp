// MexicanHat_Win32Project_MM.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include "commdlg.h"
#include <Wingdi.h>
#include "GdiPlus.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "glut.h"
#include "MexicanHat_Win32Project_MM.h"
#include <Math.h>

#define MAX_LOADSTRING 100

#define imageWidth 64
#define imageHeight 64 

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

bool gdglu = false;  //контролна променлива за използвания режим на изчертаване

// манипулатор към входното изображение
HBITMAP hbmHBITMAP1 = NULL;
//заглаван част на входното изображение 
LPBITMAPINFOHEADER m_lpBMIH1 = NULL;
// масив на входното изображение 
BYTE * image1;

OPENFILENAME ofn;
char FileName[256];
char* imname;
char path[128];
wchar_t szFilter[] = _TEXT("All Files(*.*)\0*.*\0\0");

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

//масиви за динамична светлина
GLfloat ambient_light[] = { 0.2, 0.2, 0.2, 1.0 }; // цвят за AMBIENT светлина 
GLfloat diffuse_light[] = { 1.0, 1.0, 1.0, 1.0 }; // цвят за DIFFUSE светлина 
GLfloat emission[] = { 0.8, 0.8, 0.0, 1.0 }; // цвят за EMISSION светлина  
GLfloat emission_none[] = { 0.0, 0.0, 0.0, 1.0 };// черен цвят за изключване на EMISSION 

GLfloat texpts[2][2][2] = { { { 0.0, 0.0 }, { 0.0, 1.0 } },
{ { 1.0, 0.0 }, { 1.0, 1.0 } } };

// масив на генерираното изображение
GLubyte image[3 * imageWidth * imageHeight];
void makeImage(void);

HGLRC hRC = NULL;
HDC hdc;
HWND hWnd;
float r;
int showPoints = 0;
int a = 1; // допълнителна променлива
GLfloat angleHat = 0.0f;  // Rotational angle

GLfloat light_position[] = { 0.0, 20.0, 10.0, 1.0 };  // позиция на източника 

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

long Datasize(LPBITMAPINFOHEADER m_lpBMIH);
HBITMAP LoadFileImage(wchar_t* FileName); //зареждане на външно изображение
void getDataHBITMAP(HBITMAP hbmHBITMAP); //извличане на данните за изображението
BOOL DisplayBmpJPG(HWND hWnd, HBITMAP hbmHBITMAP); //визуализация на изображението
void makeImage(void);//процедура генерираща изображението
void DefineView(int width, int height); // прототип на инициализиращата функция
void Lighting(void);              // прототип на функцията за динамична светлина
void Render_Bezier(void);         // изчертава криви на Безие
void Render_BezierSurf(void);     // изчертава равнина чрез Безие сплайни
void Render_NURBS(void);          // изчетава базисен сплайн
void Render_NURBSSurf(void);      // изчертава равнина, с изрязана област
void Render_BezierS_textura(void);
void Render_NURBS_textura(void);
void Render_Hat(void);            // изчертава мексиканска шапка с текстура

void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {

		sizeof(PIXELFORMATDESCRIPTOR), // Size of this structure
		1,                             // Version of this structure
		PFD_DRAW_TO_WINDOW |           // Draw to window (not to bitmap)
		PFD_SUPPORT_OPENGL |           // Support OpenGL calls in window
		PFD_DOUBLEBUFFER,              // Double-buffered mode
		PFD_TYPE_RGBA,                 // RGBA color mode
		32,                            // Дълбочина на цвета
		0, 0, 0, 0, 0, 0,                   // Not used to select mode

		0, 0,                           // Not used to select mode
		0, 0, 0, 0, 0,                     // Not used to select mode
		16,                            // Дълбочина на Z буфера
		0,                             // Not used here
		0,                             // Not used here
		0,                             // Not used here
		0,                             // Not used here
		0, 0, 0 };                       // Not used here

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

long Datasize(LPBITMAPINFOHEADER m_lpBMIH)
{//помощна функция за определяне на размера на масива на изображението
	long sizeImage;
	int p, dump;
	if (m_lpBMIH->biBitCount == 8) p = 1;
	if (m_lpBMIH->biBitCount == 24) p = 3;
	dump = (m_lpBMIH->biWidth*p) % 4;
	if (dump != 0) dump = 4 - dump;
	sizeImage = m_lpBMIH->biHeight*(m_lpBMIH->biWidth + dump);
	return sizeImage;
}

BOOL DisplayBmpJPG(HWND hWnd, HBITMAP hbmHBITMAP)
{  //визуализация с GDI на HBITMAP 

	Graphics g(::GetDC(hWnd));

	RECT rc;
	::GetClientRect(hWnd, &rc);

	Bitmap * pb = Bitmap::FromHBITMAP(hbmHBITMAP, NULL);
	//изчертаване на изображението в границите на прозореца
	g.DrawImage(pb, float(rc.left), float(rc.top), float(rc.right), float(rc.bottom));

	delete pb;
	return true;
}

void getDataHBITMAP(HBITMAP hbmHBITMAP)
{
	//извличане на описанието и матрицата на изображението 
	LPBYTE m_lpImage;
	DIBSECTION ds;

	::GetObject(hbmHBITMAP, sizeof(ds), &ds);
	if (m_lpBMIH1 != NULL) delete m_lpBMIH1;
	m_lpBMIH1 = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
	memcpy(m_lpBMIH1, &ds.dsBmih, sizeof(BITMAPINFOHEADER));
	if (m_lpBMIH1->biSizeImage == 0)
		m_lpBMIH1->biSizeImage = Datasize(m_lpBMIH1);

	// копиране на матрицата на изображението
	m_lpImage = (LPBYTE)ds.dsBm.bmBits;
	// масив с изображението
	image1 = m_lpImage;
}

HBITMAP LoadFileImage(wchar_t* FileName)
{
	// въвъежда изображение от файл с помощта на GDI+

	Bitmap * pbmp = Bitmap::FromFile(FileName);
	HBITMAP JPGImage;
	if (pbmp == NULL) JPGImage = NULL;
	else
		if (Ok != pbmp->GetHBITMAP(Color::Black, &JPGImage))
			JPGImage = NULL;

	delete pbmp;
	return JPGImage;
}

void makeImage(void)//процедура генерираща изображението
{
	int i, j;
	float ti, tj;
	for (i = 0; i < imageWidth; i++) {
		ti = 2.0*3.14159265*i / imageWidth;
		for (j = 0; j < imageHeight; j++) {
			tj = 2.0*3.14159265*j / imageHeight;
			image[3 * (imageHeight*i + j)] = (GLubyte)127 * (1.0 + sin(ti));
			image[3 * (imageHeight*i + j) + 1] = (GLubyte)127 * (1.0 + cos(2 * tj));
			image[3 * (imageHeight*i + j) + 2] = (GLubyte)127 * (1.0 + cos(ti + tj));
		}
	}
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

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MEXICANHAT_WIN32PROJECT_MM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEXICANHAT_WIN32PROJECT_MM));

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

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MEXICANHAT_WIN32PROJECT_MM));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MEXICANHAT_WIN32PROJECT_MM);
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
   //HWND hWnd;

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
	//PAINTSTRUCT ps;
	//HDC hdc;

	switch (message)
	{
	case WM_CREATE:

		// съхраняване на контекста на устройството
		hdc = GetDC(hWnd);

		// селектиране на пикселния формат
		SetDCPixelFormat(hdc);

		// създаване на контекст за рендиране и насочване като текущ
		hRC = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hRC);
		break;

		// Window is being destroyed, clean up
	case WM_DESTROY:

		// деселектиране на текущия контекст за рендиране и изтриване
		hdc = GetDC(hWnd);
		wglMakeCurrent(hdc, NULL);
		wglDeleteContext(hRC);

		//  допълнение в case WM_DESTROY:
		if (m_lpBMIH1 != NULL) 
			delete m_lpBMIH1;
		PostQuitMessage(0);
		break;

		//визуализация на изображението или текстурираната повърност		
	case WM_PAINT:
	{
		if (gdglu == false){
			//	hdc = BeginPaint(hWnd, &ps);	EndPaint(hWnd, &ps);
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			if (hbmHBITMAP1 != 0) DisplayBmpJPG(hWnd, hbmHBITMAP1);
			GdiplusShutdown(gdiplusToken);
		}
		else {
			//hdc = BeginPaint(hWnd, &ps);EndPaint(hWnd, &ps);
			Render_Hat();
		}
	}
	break;

		// Промяна на размера на прозореца.
	case WM_SIZE:
		// Извикване на функцията, която задава размер на прозореца и изгледа
	{
		int height = HIWORD(lParam);
		int width = LOWORD(lParam);
		DefineView(width, height);
	}
	break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			PostQuitMessage(0);
			DestroyWindow(hWnd);
			break;

			//допълнителни бутони
		case ID_LOADIMAGE:
		{
			wchar_t defex[] = __TEXT("*.*");
			*FileName = 0;
			RtlZeroMemory(&ofn, sizeof ofn);
			ofn.lStructSize = sizeof ofn;
			ofn.hwndOwner = hWnd;
			ofn.hInstance = hInst;
			ofn.lpstrFile = (LPWSTR)FileName;
			ofn.nMaxFile = sizeof FileName;
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
			ofn.lpstrFilter = (LPWSTR)szFilter;
			ofn.lpstrDefExt = (LPWSTR)defex;
			ofn.lpstrCustomFilter = (LPWSTR)szFilter;
			ofn.lpstrInitialDir = (LPCWSTR)path;

			if (GetOpenFileName(&ofn) != 0)
			{
				size_t origsize = wcslen((LPWSTR)FileName) + 1;
				size_t convertedChars = 0;
				char strConcat[] = " (char *)";
				size_t strConcatsize = (strlen(strConcat) + 1) * 2;
				const size_t newsize = origsize * 2;
				imname = new char[newsize + strConcatsize];
				wcstombs_s(&convertedChars, imname, newsize, (LPWSTR)FileName, _TRUNCATE);

				GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
				hbmHBITMAP1 = LoadFileImage((LPWSTR)FileName);
				getDataHBITMAP(hbmHBITMAP1);
				GdiplusShutdown(gdiplusToken);
				delete[] imname;
			}
		}
		break;

		case ID_TEXTURE:
			gdglu = true;
			break;

		}
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

void DefineView(int width, int height)
{
	if (height == 0) height = 1;

	// задаване на изглед върху целия екран 
	glViewport(0, 0, width, height);

	// зареждане на матрицата на проекцията
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// определяне на типа на проекцията  
	gluPerspective((GLdouble)45.0, (GLfloat)width / (GLfloat)height, 0.1, 200.0);

	// зареждане на моделната матрица         
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//зареждане на параметрите на динамичната светлина 
	Lighting();

	//определяне на разстоянията до обектите
	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);   // Enable smooth shading

	//задаване на фонов цвят
	glClearColor(0.2, 0.2, 0.5, 0.0);
}

void Lighting(void)
{
	// осветление

	// включване на динамичната светлина
	glEnable(GL_LIGHTING);

	// глобална подсветка
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);

	// определяне на осветяване на фигурите при AMBIENT и DIFFUSE светлина
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_light);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_light);

	// задаване на запазване на цвета на обекта при включване на динамичната светлина
	glEnable(GL_COLOR_MATERIAL);

	// определяне на видовете светлина участващи в крайното оцветяване
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void Render_Bezier(void) // изчетава криви на Безие
{
	//изтриване на съдържанието на буферите
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity(); // начална инициализация   
	//задаване на осветлението
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialf(GL_FRONT, GL_DIFFUSE, 50.0);
	glEnable(GL_COLOR_MATERIAL);

	//определяне на позицията на наблюдателя
	gluLookAt(0.0, -10.0, -50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//задаване на движение на управляващите точки
	r += 0.002; if (r >= 23.0) r = 0.0;
	GLfloat ctrlpoints[7][3]; //масив на контролните точки
	//задаване на стойности на контролните и управляващите точки
	ctrlpoints[0][0] = -4.0; ctrlpoints[0][1] = -4.0; ctrlpoints[0][2] = 0.0;
	ctrlpoints[1][0] = -2.0 + r; ctrlpoints[1][1] = 4.0 - r; ctrlpoints[1][2] = 0.0 + r;
	ctrlpoints[2][0] = 2.0 - r; ctrlpoints[2][1] = -4.0 - 0.2* r; ctrlpoints[2][2] = 0.0;
	ctrlpoints[3][0] = 4.0; ctrlpoints[3][1] = 4.0; ctrlpoints[3][2] = 0.0;
	ctrlpoints[4][0] = 6.0 - r; ctrlpoints[4][1] = 6.0 + 0.5*r; ctrlpoints[4][2] = 0.0 + r;
	ctrlpoints[5][0] = -2.0 + r; ctrlpoints[5][1] = 8.0 - r; ctrlpoints[5][2] = 0.0;
	ctrlpoints[6][0] = -4.0; ctrlpoints[6][1] = -4.0; ctrlpoints[6][2] = 0.0;

	// равномерно оцветяване на обекта
	glShadeModel(GL_FLAT);
	int i;
	//съставяне на карта 
	glMap1f(GL_MAP1_COLOR_4, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
	glEnable(GL_MAP1_COLOR_4);
	//определяне на цвят на линията
	glColor3f(1.0, 0.0, 0.0);

	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
	glEnable(GL_MAP1_VERTEX_3);

	glBegin(GL_LINE_STRIP);
	//пресмятане на координатите на точките по кривата
	for (i = 0; i <= 30; i++)
		glEvalCoord1f((GLfloat)i / 30.0);
	glEnd();

	// алтернативен вариант на изчертаване чрез решетка на първата линия
	glPointSize(3.0); //размер на точките по линията
	glMapGrid1f(30, 0.0, 1.0); //30 интервала
	//визуализация на кривата с точки без първата и последната 
	glEvalMesh1(GL_POINT, 1, 29);

	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[3][0]);
	glEnable(GL_MAP1_VERTEX_3);

	glBegin(GL_LINE_STRIP);
	//пресмятане на координатите на точките по кривата
	for (i = 0; i <= 30; i++)
		glEvalCoord1f((GLfloat)i / 30.0);
	glEnd();

	// алтернативен вариант на изчертаване чрез решетка на втората линия
	glPointSize(3.0); //размер на точките по линията
	glMapGrid1f(30, 0.0, 1.0); //30 интервала
	//визуализация на кривата с точки без първата и последната 
	glEvalMesh1(GL_POINT, 1, 29);

	glBegin(GL_LINE_STRIP);
	//пресмятане на координатите на точките по кривата
	for (i = 0; i <= 30; i++)
		glEvalCoord1f((GLfloat)i / 30.0);
	glEnd();


	// задаване на возуализация на крайните и управляващите точки    
	glDisable(GL_LIGHTING);
	glPointSize(5.0); //размер на контролните точки
	glColor3f(1.0, 0.0, 1.0); //цвят на точките	

	glBegin(GL_POINTS);
	for (i = 0; i < 7; i++)
		glVertex3fv(&ctrlpoints[i][0]);
	glEnd();
	//смяна на буферите		
	SwapBuffers(hdc);
	//връщане на управлението към системата
	ReleaseDC(hWnd, hdc);
}

void Render_BezierSurf(void)//изчертава равнина чрез Безие сплайни
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	r += 0.001; if (r >= 6) r = 0.0;
	//задаване на осветлението
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialf(GL_FRONT, GL_DIFFUSE, 50.0);
	glEnable(GL_COLOR_MATERIAL);

	//определяне на позицията на наблюдателя
	gluLookAt(0.0, 0.0, -15.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//масив от контролни точки
	GLfloat ctrlpoints[4][4][3] = {
		{ { -1.5, -1.5, 4.0 }, { -0.5, -1.5, 2.0 },
		{ 0.5, +1.5, -1.0 }, { 1.5, -1.5, 2.0 } },
		{ { -1.5, -0.5 + r, 1.0 }, { -0.5, -0.5 + r, 3.0 },
		{ 0.5, -0.5 + r, 0.0 }, { 1.5, -0.5 + r, -1.0 } },
		{ { -1.5, 0.5 + r, 4.0 }, { -0.5, 0.5 + r, 0.0 },
		{ 0.5, 0.5 + r, 3.0 }, { 1.5, 0.5 + r, 4.0 } },
		{ { -1.5, 1.5, -2.0 }, { -0.5, 1.5, -2.0 },
		{ 0.5, 1.5, 0.0 }, { 1.5, 1.5, -1.0 } }
	};

	//формиране на карта
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);

	//определяне на оцветяването
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glColor3f(0.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	int i, j;
	//изчисляване и визуализация на точките от равнината
	glPushMatrix();//задаване на ротация
	glRotatef(r * 60, 0.0, 1.0, 0.0);

	//изчертаване на линии по повърхнината
	for (j = 0; j <= 8; j++) {
		glBegin(GL_LINES);
		for (i = 0; i <= 30; i++)
			glEvalCoord2f((GLfloat)i / 30.0, (GLfloat)j / 8.0);
		glEnd();
		glBegin(GL_LINES);
		for (i = 0; i <= 30; i++)
			glEvalCoord2f((GLfloat)j / 8.0, (GLfloat)i / 30.0);
		glEnd();
	}

	/* //изчертаване на плътна повърхина
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	*/

	glPopMatrix();
	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);
}

void Render_NURBS(void) // изчетава базисен сплайн
{	//масив от контролни точки
	GLfloat ctlpoints[7][4][3] = {
		{ { -1.5, -2.5, 3.0 }, { -0.5, -2.5, 2.0 },
		{ 0.5, -2.5, -1.0 }, { 1.5, -2.5, 2.0 } },
		{ { -1.5, -1.5, 1.0 }, { -0.5, -1.5, 3.0 },
		{ 0.5, -1.5, 0.0 }, { 1.5, -1.5, -1.0 } },
		{ { -1.5, 0.5, 3.0 }, { -0.5, 0.5, 0.0 },
		{ 0.5, 0.5, 3.0 }, { 1.5, 0.5, 4.0 } },
		{ { -1.5, 2.5, -2.0 }, { -0.5, 2.5, -2.0 },
		{ 0.5, 2.5, 0.0 }, { 1.5, 2.5, -1.0 } },
		{ { -1.5, -1.5, 3.0 }, { -0.5, -1.5, 2.0 },
		{ 0.5, -1.5, -1.0 }, { 1.5, -1.5, 2.0 } },
		{ { -1.5, -1.5, 1.0 }, { -0.5, -1.5, 3.0 },
		{ 0.5, -1.5, 0.0 }, { 1.5, -1.5, -1.0 } },
		{ { -1.5, 1.5, 3.0 }, { -0.5, 1.5, 0.0 },
		{ 0.5, 1.5, 3.0 }, { 1.5, 1.5, 4.0 } }
	};

	r += 0.02; 	if (r >= 360.0)
	{
		r = 0.00; a++;
	}
	//масив от възли	
	GLfloat knots[11] = { 0.0, 0.0, 0.0, 0.0, 0.34, 0.58, 0.76, 1.0, 1.0, 1.0, 1.0 };
	//изтриване на съдържанието на буферите
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();// начална инициализация   
	//задаване на осветлението
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialf(GL_FRONT, GL_DIFFUSE, 50.0);
	glEnable(GL_COLOR_MATERIAL);
	//определяне на позицията на наблюдателя
	gluLookAt(0.0, 0.0, 13.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//създаване на обект
	GLUnurbsObj *theNurb;
	theNurb = gluNewNurbsRenderer();
	//описание на характеристиките на обекта
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 20.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_PATCH);
	// ротация на обекта
	glPushMatrix();
	glRotatef(r, 0.0, 1.0, 0.0);
	//цвят на обекта
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 0.0, 1.0);
	//изчертаване на кривата
	gluBeginCurve(theNurb);
	if (a > 4) a = 1;
	gluNurbsCurve(theNurb, 11, knots, a * 3, &ctlpoints[0][0][0], 4, GL_MAP1_VERTEX_3);
	gluEndCurve(theNurb);
	glPopMatrix();
	gluDeleteNurbsRenderer(theNurb);
	//смяна на буферите		
	SwapBuffers(hdc);
	//връщане на управлението към системата
	ReleaseDC(hWnd, hdc);
}

void Render_NURBSSurf(void)//изчертава равнина, с изрязана област
{
	//изчистване на буферите и начална инициализация 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//осветяване с насочен източник
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	GLfloat position[] = { -5.0, -10.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);  glMaterialf(GL_FRONT, GL_DIFFUSE, 30.0);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	//определяне на позицията на наблюдателя
	gluLookAt(0.0, 0.0, -15.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	GLfloat ctlpoints[4][4][3]; //масив с контролни точки
	int u, v;
	for (u = 0; u < 4; u++) {
		for (v = 0; v < 4; v++) {
			ctlpoints[u][v][0] = 2.0*((GLfloat)u - 1.5);
			ctlpoints[u][v][1] = 2.0*((GLfloat)v - 1.5);
			if (((u == 1) || (u == 2)) && ((v == 1) || (v == 2)))
				ctlpoints[u][v][2] = 3.0;
			else
				ctlpoints[u][v][2] = -3.0;
		}
	}

	//масив от възли
	GLfloat knots[8] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };

	//масиви на линиите на изрязване
	GLfloat edgePt[5][2] = /* counter clockwise */
	{ { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0 } };
	GLfloat curvePt[4][2] = /* clockwise */
	{ { 0.25, 0.5 }, { 0.25, 0.75 }, { 0.75, 0.75 }, { 0.75, 0.5 } };
	GLfloat curveKnots[8] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };
	GLfloat pwlPt[4][2] = /* clockwise */{ { 0.75, 0.5 }, { 0.5, 0.25 }, { 0.25, 0.5 } };

	r += 0.5; if (r >= 360) r = 0.0;

	//създаване на обект
	GLUnurbsObj *theNurb;
	theNurb = gluNewNurbsRenderer();
	//задаване на характеристики
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

	//визуализация с ротация 
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 0.0);
	glRotatef(r, 0.0, 1.0, 0.0);
	gluBeginSurface(theNurb);
	gluNurbsSurface(theNurb, 8, knots, 8, knots, 4 * 3, 3, &ctlpoints[0][0][0], 4, 4, GL_MAP2_VERTEX_3);

	//изрязване
	gluBeginTrim(theNurb);
	gluPwlCurve(theNurb, 5, &edgePt[0][0], 2, GLU_MAP1_TRIM_2);
	gluEndTrim(theNurb);
	gluBeginTrim(theNurb);
	gluNurbsCurve(theNurb, 8, curveKnots, 2, &curvePt[0][0], 4, GLU_MAP1_TRIM_2);
	gluPwlCurve(theNurb, 3, &pwlPt[0][0], 2, GLU_MAP1_TRIM_2);
	gluEndTrim(theNurb);
	gluEndSurface(theNurb);

	showPoints = true;

	if (showPoints) {
		glPointSize(3.0);
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				glVertex3f(ctlpoints[i][j][0],
					ctlpoints[i][j][1], ctlpoints[i][j][2]);
			}
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();
	gluDeleteNurbsRenderer(theNurb);

	//смяна на буферите		
	SwapBuffers(hdc);
	//връщане на управлението към системата
	ReleaseDC(hWnd, hdc);
}

void Render_BezierS_textura(void)
//изчертава текстура върху въртяща се равнина,формирана със сплайни на Безие
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	GLfloat position[] = { -5.0, -10.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);   glMaterialf(GL_FRONT, GL_DIFFUSE, 30.0);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	gluLookAt(0.0, 3.0, -15.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	r += 0.5; if (r >= 360) r = 0.0;
	GLfloat ctrlpoints[4][4][3] = {
		{ { -1.5, -1.5, 4.0 }, { -0.5, -1.5, 2.0 },
		{ 0.5, +1.5, -1.0 }, { 1.5, -1.5, 2.0 } },
		{ { -1.5, -0.5, 1.0 }, { -0.5, -0.5, 3.0 },
		{ 0.5, -0.5, 0.0 }, { 1.5, -0.5, -1.0 } },
		{ { -1.5, 0.5, 4.0 }, { -0.5, 0.5, 0.0 },
		{ 0.5, 0.5, 3.0 }, { 1.5, 0.5, 4.0 } },
		{ { -1.5, 1.5, -2.0 }, { -0.5, 1.5, -2.0 },
		{ 0.5, 1.5, 0.0 }, { 1.5, 1.5, -1.0 } }
	};

	//създаване на крата за текстурата
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glEnable(GL_MAP2_VERTEX_3);
	//създаване на мрежа за повърхнината
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);

	makeImage();//генериране на изображение

	//определяне на начина на повторение в различните направления
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	//изчертаване на повърхнината с нанесена текстура
	glPushMatrix();
	glRotatef(r, 1.0, 1.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();


	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);
}

void Render_NURBS_textura(void)
//изчертава текстура върху въртяща се равнина, формирана с базисни сплайни 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	GLfloat position[] = { -5.0, -10.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);  glMaterialf(GL_FRONT, GL_DIFFUSE, 30.0);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	gluLookAt(0.0, 0.0, -15.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	GLfloat ctlpoints[4][4][3]; //масив с контролни точки
	int u, v;
	for (u = 0; u < 4; u++) {
		for (v = 0; v < 4; v++) {
			ctlpoints[u][v][0] = 2.0*((GLfloat)u - 1.5);
			ctlpoints[u][v][1] = 2.0*((GLfloat)v - 1.5);
			if (((u == 1) || (u == 2)) && ((v == 1) || (v == 2)))
				ctlpoints[u][v][2] = 3.0;
			else
				ctlpoints[u][v][2] = -3.0;
		}
	}

	//масив с възли

	GLfloat knots[8] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };
	r += 0.5; if (r >= 360) r = 0.0;

	//създаване на крата за текстурата

	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);

	makeImage(); //генериране на изображение

	//определяне на начина на повторение в различните направления
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	//създаване на обект и задаване на параметри
	GLUnurbsObj *theNurb;
	theNurb = gluNewNurbsRenderer();
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);


	//изчертаване на повърхнината с нанесена текстура

	glPushMatrix();
	glRotatef(r, 0.0, 1.0, 0.0);

	gluBeginSurface(theNurb);
	gluNurbsSurface(theNurb, 8, knots, 8, knots, 4 * 3, 3, &ctlpoints[0][0][0], 4, 4, GL_MAP2_VERTEX_3);

	gluEndSurface(theNurb);
	glPopMatrix();
	gluDeleteNurbsRenderer(theNurb);

	// смяна на буферите
	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);
}

void Render_Hat(void)
//изчертава външно изображение върху въртяща се равнина, формирана със сплайни на Безие
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	GLfloat position[] = { -5.0, -10.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);  glMaterialf(GL_FRONT, GL_DIFFUSE, 30.0);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	gluLookAt(0.0, 3.0, -20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	r += 0.5; if (r >= 360) r = 0.0;
	
	GLfloat ctrlpoints[6][6][3] =
	{ { { -4, 0, -4 }, { -4.5, 0, -3 }, { -5, 0, -1 }, { -5, 0, -1 }, { -5, 0, 1 }, { -4, 0, 4 } },
	{ { -3, 0, -5 }, { -3, -3, -3 }, { -3, -3, -1 }, { -3, -3, 1 }, { -3, -3, 3 }, { -3, 0, 5 } },
	{ { -1, 0, -5 }, { -1, -3, -3 }, { -1, 7, -1 }, { -1, 7, 1 }, { -1, -3, 3 }, { -1, 0, 5 } },
	{ { 1, 0, -5 }, { 1, -3, -3 }, { 1, 7, -1 }, { 1, 7, 1 }, { 1, -3, 3 }, { 1, 0, 5 } },
	{ { 3, 0, -5 }, { 3, -3, -3 }, { 3, -3, -1 }, { 3, -3, 1 }, { 3, -3, 3 }, { 3, 0, 5 } },
	{ { 4, 0, -4 }, { 5, 0, -3 }, { 5, 0, -1 }, { 5, 0, 1 }, { 5, 0, 3 }, { 4, 0, 4 } }
	};

	/*//създаване на крата за текстурата
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glEnable(GL_MAP2_VERTEX_3);*/

	//създаване на мрежа за повърхнината
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);

	//определяне на начина на повторение в различните направления
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int imageWidth1 = m_lpBMIH1->biWidth;
	int imageHeight1 = m_lpBMIH1->biHeight;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth1, imageHeight1, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	//масив от възли
	GLfloat knots[10] = { 0.0, 0.0, 0.0, 0.0, 0.2, 0.8, 1.0, 1.0, 1.0, 1.0 };

	//създаване на обект
	GLUnurbsObj *theNurb;
	theNurb = gluNewNurbsRenderer();

	//задаване на характеристики
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

	//визуализация с ротация 
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 0.9, 0.6);
	glRotatef(angleHat, 1.0, 1.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	gluBeginSurface(theNurb);
	gluNurbsSurface(theNurb, 10, knots, 10, knots, 6 * 3, 3, &ctrlpoints[0][0][0], 4, 4, GL_MAP2_VERTEX_3);

	//изрязване
	//gluBeginTrim(theNurb);
	//gluPwlCurve(theNurb, 5, &edgePt[0][0], 2, GLU_MAP1_TRIM_2);
	//gluEndTrim(theNurb);
	//gluBeginTrim(theNurb);
	//gluNurbsCurve(theNurb, 8, curveKnots, 2, &curvePt[0][0], 4, GLU_MAP1_TRIM_2);
	//gluPwlCurve(theNurb, 3, &pwlPt[0][0], 2, GLU_MAP1_TRIM_2);
	//gluEndTrim(theNurb);
	gluEndSurface(theNurb);

	showPoints = true;

	if (showPoints) {
		glPointSize(3.0);
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 6; j++) {
				glVertex3f(ctrlpoints[i][j][0],
					ctrlpoints[i][j][1], ctrlpoints[i][j][2]);
			}
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();
	gluDeleteNurbsRenderer(theNurb);

	//смяна на буферите		
	SwapBuffers(hdc);
	angleHat += 0.1;
	//връщане на управлението към системата
	ReleaseDC(hWnd, hdc);
}
