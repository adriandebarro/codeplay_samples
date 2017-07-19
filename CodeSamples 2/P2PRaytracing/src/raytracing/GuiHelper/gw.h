// File gw.h - M. Covington 1996
// A graphics window for console applications

#ifndef __GW__
#define __GW__


#include <windows.h>        // for Windows API


#ifndef _INC_CONIO
#include <conio.h>          // for kbhit() only
#endif
#include <stdio.h>

// Globals

int gwWidth, gwHeight;   // Size of drawing area
static HWND gwHwnd = NULL;     // Handle to window
static HDC GW = NULL;     // Device context on which we draw
HBITMAP gwBitmap;           // Bitmap that holds its contents

// [KB] Double-buffering with DIB direct memory access
struct DIBElement
{
  unsigned char blue;
  unsigned char green;
  unsigned char red;
  unsigned char alpha;
};

DIBElement *dibRender = nullptr;
HBITMAP dibSection = 0;
HDC dibHDC = 0;
// [KB] Double-buffering with DIB direct memory access

// gwWndProc(...)
//
// Window procedure for the child window.
// Called by Windows, not by user's program.

void Paint(HDC);

LRESULT CALLBACK gwWndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	//help
	HDC hdc;
	PAINTSTRUCT ps;

	if (message == WM_DESTROY)
	{
		// Before destroying the window,
		// delete resources that belong to it.
		SelectObject(GW, gwBitmap);
		DeleteDC(GW);
		DeleteObject(gwBitmap);
		GW = NULL;   // update globals
		gwHwnd = NULL;
		PostQuitMessage(0);
		return 0;
	}
	else if (message == WM_PAINT)
	{
		// To paint the window, blit the bitmap into it
		hdc = BeginPaint(gwHwnd, &ps);
		// BitBlt(hdc, 0, 0, gwWidth, gwHeight, GW, 0, 0, SRCCOPY);
    	Paint(hdc);
		EndPaint(gwHwnd, &ps);
		return 0;
	}
	else
		return (DefWindowProc(hWnd, message, wParam, lParam));
}


// gwDoEvents()
//
// Procedure to handle events so the graphics window doesn't
// "freeze."  Main program must call this REPEATEDLY when not
// doing anything else.

void gwDoEvents()
{
	// Get the console output up to date
#if defined(_INC_OSTREAM)
	cout.flush();
#endif
	fflush(stdout);
	fflush(stderr);

	// If graphics window is not open, exit harmlessly
	if (!gwHwnd) return;

	// Get the contents of the window up to date
	InvalidateRect(gwHwnd, NULL, FALSE);
	UpdateWindow(gwHwnd);

	// If the window has messages pending,
	// pass them along to be processed
	MSG msg;
	if (PeekMessage(&msg, gwHwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

// gwWaitForKey()
//
// Calls gwDoEvents() repeatedly until a key is pressed.

void gwWaitForKey()
{
	while (!_kbhit()) gwDoEvents();
}


// gwClose()
//
// Removes the graphics window.  Optional at end of program.

void gwClose()
{
	DestroyWindow(gwHwnd);  // sends WM_DESTROY message
}


// gwOpen(title,width,height)
//
// Creates and open the graphics window.
// Initializes globals gwHeight, gwWidth, gwHwnd, gwBitmap, GW.

void gwOpen(LPSTR title, int width, int height)
{
	WNDCLASS  wndclass;     // Window class
	HINSTANCE hinstance;    // Application instance
	RECT      wrect, crect; // Window size data
	HDC       hdc;          // Window device context

	if (gwHwnd) gwClose();  // close the existing graphics window

	// Initialize globals
	gwWidth = width;
	gwHeight = height;

	// Windows 95/NT trick to get application instance
	hinstance = GetModuleHandle(NULL);

	// Register the window class
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = gwWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hinstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)BLACK_BRUSH;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = L"gwWindow";
	RegisterClass(&wndclass);

	// Create the window
	gwHwnd = CreateWindow(
		L"gwWindow",         // class name
		L"Render Window",	// title
		WS_OVERLAPPED,      // style
		CW_USEDEFAULT,      // x position
		CW_USEDEFAULT,      // y position
		width,              // x size
		height,             // y size
		GetActiveWindow(),  // parent
		NULL,               // menu ptr
		hinstance,          // app instance
		NULL);              // init params

	// Adjust the window size.  User gave the size of the
	// client area; we need to know the size of the whole thing.
	// Unlike AdjustWindowRect, this algorithm takes the border
	// and title bar into account.
	GetClientRect(gwHwnd, &crect);
	GetWindowRect(gwHwnd, &wrect);
	SetWindowPos(gwHwnd, HWND_TOP, wrect.left, wrect.top,
		wrect.right - wrect.left + width - crect.right,
		wrect.bottom - wrect.top + height - crect.bottom,
		0);

  hdc = GetDC(gwHwnd);            // dc of window

  // Create DIB Section
  BITMAPINFO dibBitmapInfo;
  memset(&dibBitmapInfo, 0, sizeof(dibBitmapInfo));
  dibBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  dibBitmapInfo.bmiHeader.biCompression = BI_RGB;
  dibBitmapInfo.bmiHeader.biSizeImage = 0;
  dibBitmapInfo.bmiHeader.biWidth = gwWidth;
  dibBitmapInfo.bmiHeader.biHeight = -gwHeight;
  dibBitmapInfo.bmiHeader.biBitCount = 32;
  dibBitmapInfo.bmiHeader.biPlanes = 1;

  dibHDC = CreateCompatibleDC(hdc);
  dibSection = CreateDIBSection(hdc,
    &dibBitmapInfo, DIB_RGB_COLORS, (void**)&dibRender, 
    NULL, NULL);

	// Create a bitmap in memory to hold the graphics
	GW = CreateCompatibleDC(hdc);   // dc of memory bitmap
	gwBitmap = CreateCompatibleBitmap(hdc, width, height);

  ReleaseDC(gwHwnd, hdc);
	SelectObject(GW, gwBitmap);

	// Fill the bitmap with white
	BitBlt(GW, 0, 0, gwWidth, gwHeight, NULL, 0, 0, WHITENESS);

	// Put the window on the screen
	InvalidateRect(gwHwnd, NULL, FALSE);
	ShowWindow(gwHwnd, SW_SHOWNORMAL);
}


HDC GetHandler()
{
	return GW;
}

void SetColor(int p_x, int p_y, float p_r, float p_g, float p_b)
{
  DIBElement *dibPixel = dibRender + p_x + p_y * gwWidth;

  dibPixel->red = (unsigned char)(p_r);
  dibPixel->green = (unsigned char)(p_g);
  dibPixel->blue = (unsigned char)(p_b);
}

void Paint(HDC p_hdc = 0)
{
  SelectObject(dibHDC, dibSection);

  assert(p_hdc != 0);
  assert(BitBlt(p_hdc, 0, 0, gwWidth, gwHeight, dibHDC, 0, 0, SRCCOPY) != 0);
}

void CloseWindow()
{
	CloseWindow(gwHwnd);
}

#endif 

// End of gw.h
