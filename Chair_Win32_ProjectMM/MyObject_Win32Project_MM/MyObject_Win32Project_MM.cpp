// MyObject_Win32Project_MM.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MyObject_Win32Project_MM.h"
#include <Wingdi.h>
#include "GdiPlus.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "glut.h"
#include <Math.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//масиви за динамична светлина
GLfloat ambient_light[] = { 0.2, 0.2, 0.2, 1.0 }; // цвят за AMBIENT светлина 
GLfloat diffuse_light[] = { 1.0, 1.0, 1.0, 1.0 }; // цвят за DIFFUSE светлина 
GLfloat emission[] = { 0.8, 0.8, 0.0, 1.0 }; // цвят за EMISSION светлина  
GLfloat emission_none[] = { 0.0, 0.0, 0.0, 1.0 };// черен цвят за изключване на EMISSION 

HGLRC hRC = NULL;
HDC hdc;
HWND hWnd;
float angle = 0.0;

GLfloat light_position[] = { 0.0, 20.0, 10.0, 1.0 };  // позиция на източника 

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void DefineView(int width, int height);           // прототип на инициализиращата функция
void Lighting(void);                              // прототип на функцията за динамична светлина
void Render_MyObject(void);                       // изчертава избрания обект

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYOBJECTWIN32PROJECTMM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYOBJECTWIN32PROJECTMM));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
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
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYOBJECTWIN32PROJECTMM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYOBJECTWIN32PROJECTMM);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
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

        //приложението спира след затваряне на прозореца  
        PostQuitMessage(0);
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
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        //hdc = BeginPaint(hWnd, &ps);EndPaint(hWnd, &ps);
        Render_MyObject();	// TODO: Add any drawing code here...

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
    glClearColor(0, 0, 0, 0);
}

void Lighting(void)
{
    // осветление

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

void Render_MyObject(void) 
{
	//изтриване на съдържанието на буферите
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); // зареждане на параметри   
	glTranslatef(-1.5f, 0.0f, -6.0f);

	// определяне на DIFFUSE излъчване
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);

	// определяне на позиция на източника
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 70.0);

	// включване на светлинен източник
	glEnable(GL_LIGHT0);

	//определяне на позицията на наблюдателя
	gluLookAt(0.0, 0.0, 15.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// поставяне на моделната матрица за обслужване в стека с фигурата
	glPushMatrix();
	glRotatef(angle, 1.0, 1.0, 1.0);

	glColor3f(1.0, 0.9, 0.6);
	glBegin(GL_QUADS);

	//Front
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-2.0f, -0.2f, 2.0f);
	glVertex3f(2.0f, -0.2f, 2.0f);
	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, 2.0f);

	//Right
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(2.0f, -0.2f, 2.0f);

	//Back
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-2.0f, -0.2f, -2.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);

	//Left
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-2.0f, -0.2f, -2.0f);
	glVertex3f(-2.0f, -0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);

	//Top
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);

	//Bottom
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(2.0f, -0.2f, 2.0f);
	glVertex3f(-2.0f, -0.2f, 2.0f);
	glVertex3f(-2.0f, -0.2f, -2.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);

//Legs

	//Front
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.8f, -0.2f, 1.6f);
	glVertex3f(1.4f, -0.2f, 1.6f);
	glVertex3f(1.4f, -3.0f, 1.6f);
	glVertex3f(1.8f, -3.0f, 1.6f);

	//Back
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(1.8f, -0.2f, 1.2f);
	glVertex3f(1.4f, -0.2f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.2f);

	//Right
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.8f, -0.2f, 1.6f);
	glVertex3f(1.8f, -0.2f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.6f);

	//Left
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.4f, -0.2f, 1.6f);
	glVertex3f(1.4f, -0.2f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.6f);


	//Front
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(1.8f, -0.2f, -1.2f);
	glVertex3f(1.4f, -0.2f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.2f);

	//Back
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(1.8f, -0.2f, -1.6f);
	glVertex3f(1.4f, -0.2f, -1.6f);
	glVertex3f(1.4f, -3.0f, -1.6f);
	glVertex3f(1.8f, -3.0f, -1.6f);

	//Right
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.8f, -0.2f, -1.6f);
	glVertex3f(1.8f, -0.2f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.6f);

	//Left
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.4f, -0.2f, -1.6f);
	glVertex3f(1.4f, -0.2f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.6f);


	//Front
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.8f, -0.2f, 1.6f);
	glVertex3f(-1.4f, -0.2f, 1.6f);
	glVertex3f(-1.4f, -3.0f, 1.6f);
	glVertex3f(-1.8f, -3.0f, 1.6f);

	//Back
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-1.8f, -0.2f, 1.2f);
	glVertex3f(-1.4f, -0.2f, 1.2f);
	glVertex3f(-1.4f, -3.0f, 1.2f);
	glVertex3f(-1.8f, -3.0f, 1.2f);

	//Right
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.8f, -0.2f, 1.6f);
	glVertex3f(-1.8f, -0.2f, 1.2f);
	glVertex3f(-1.8f, -3.0f, 1.2f);
	glVertex3f(-1.8f, -3.0f, 1.6f);

	//Left
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-1.4f, -0.2f, 1.6f);
	glVertex3f(-1.4f, -0.2f, 1.2f);
	glVertex3f(-1.4f, -3.0f, 1.2f);
	glVertex3f(-1.4f, -3.0f, 1.6f);


	//Front
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-1.8f, -0.2f, -1.2f);
	glVertex3f(-1.4f, -0.2f, -1.2f);
	glVertex3f(-1.4f, -3.0f, -1.2f);
	glVertex3f(-1.8f, -3.0f, -1.2f);

	//Back
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-1.8f, -0.2f, -1.6f);
	glVertex3f(-1.4f, -0.2f, -1.6f);
	glVertex3f(-1.4f, -3.0f, -1.6f);
	glVertex3f(-1.8f, -3.0f, -1.6f);

	//Right
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.8f, -0.2f, -1.6f);
	glVertex3f(-1.8f, -0.2f, -1.2f);
	glVertex3f(-1.8f, -3.0f, -1.2f);
	glVertex3f(-1.8f, -3.0f, -1.6f);

	//Left
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-1.4f, -0.2f, -1.6f);
	glVertex3f(-1.4f, -0.2f, -1.2f);
	glVertex3f(-1.4f, -3.0f, -1.2f);
	glVertex3f(-1.4f, -3.0f, -1.6f);

//Chair back

	//Front
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-2.0f, 0.2f, -1.8f);
	glVertex3f(2.0f, 0.2f, -1.8f);
	glVertex3f(1.0f, 3.5f, -1.8f);
	glVertex3f(-1.0f, 3.5f, -1.8f);
	
	//Back
    glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(1.0f, 3.5f, -2.0f);
	glVertex3f(-1.0f, 3.5f, -2.0f);

	glColor3f(1.0, 0.9, 0.3);

	glNormal3f(-1.0f, 0.0f, 0.0f);

	glVertex3f(-2.0f, 0.2f, -2.0f);
	glVertex3f(-1.8f, 3.5f, -2.0f);
	glVertex3f(-1.8f, 3.5f, -1.8f);
	glVertex3f(-2.0f, 0.2f, -1.8f);
	
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(1.8f, 3.5f, -2.0f);
	glVertex3f(1.8f, 3.5f, -1.8f);
	glVertex3f(2.0f, 0.2f, -1.8f);
	
	glVertex3f(-1.8f, 3.5f, -2.0f);
	glVertex3f(-1.8f, 3.5f, -1.8f);
	glVertex3f(1.8f, 3.5f, -1.8f);
	glVertex3f(1.8f, 3.5f, -2.0f);
	
	glEnd();

	//извеждане на матрицата от стека
	glPopMatrix();

	//смяна на буферите		
	SwapBuffers(hdc);

	angle += 0.03;

	//връщане на управлението към системата
	ReleaseDC(hWnd, hdc);
}