#include "pch.h"
#include <iostream>
#include "cthread.h"
#include <windowsx.h>
#include <tchar.h>
#include <time.h>

#define THREADS_NUMBER  4
#define WINDOWS_NUMBER  1
#define SLEEP_TIME      500

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


class Thread :public CThread
{
protected:
	virtual void Run(LPVOID lpParameter = 0);
};


void Thread::Run(LPVOID lpParameter)
{
	//Ramdomly generate seed
	srand((unsigned)time(NULL));

	HWND hWnd = (HWND)GetUserData();
	RECT rect;

	//Get the size of window
	BOOL bError = GetClientRect(hWnd, &rect);
	if (!bError)
	{
		return;
	}
	int iClientX = rect.right - rect.left;
	int iClientY = rect.bottom - rect.top;

	//If no dimension,don't draw
	if ((!iClientX) || (!iClientY))
	{
		return;
	}


	//Get the text of device
	HDC hDC = GetDC(hWnd);

	if (hDC)
	{
		//Draw 10 random figures
		for (int m = 0;m < WINDOWS_NUMBER;m++)
		{
			//Set the coordinates
			int iStartX = (int)(rand() % iClientX);
			int iStopX = (int)(rand() % iClientX);
			int iStartY = (int)(rand() % iClientY);
			int iStopY = (int)(rand() % iClientY);

			//Set color
			int iRed = rand() & 255;
			int iGreen = rand() & 255;
			int iBlue = rand() & 255;

			//Create a solid brush
			HANDLE hBrush = CreateSolidBrush(GetNearestColor(hDC, RGB(iRed, iGreen, iBlue)));
			HANDLE hbrOld = SelectBrush(hDC, hBrush);

			Rectangle(hDC, min(iStartX, iStopX), max(iStartX, iStopX), min(iStartY, iStopY), max(iStartY, iStopY));
			//Delete the brush
			DeleteBrush(SelectBrush(hDC, hbrOld));
		}

		//Release the context of device
		ReleaseDC(hWnd, hDC);
	}

	Sleep(SLEEP_TIME);
	return;
}



int WINAPI main(HINSTANCE hThis, HINSTANCE hPrev, LPTSTR szCommandLine, int iCmdShow)
{
    //std::cout << "Hello World!\n"; 

	WNDCLASSEX wndEx = { 0 };

	wndEx.cbClsExtra = 0;
	wndEx.cbSize = sizeof(WNDCLASSEX);
	wndEx.cbWndExtra = 0;
	wndEx.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wndEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndEx.hInstance = hThis;////
	wndEx.lpfnWndProc = WindowProcedure;
	wndEx.lpszClassName = L"async_thread";
	wndEx.lpszMenuName = NULL;
	wndEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

	if (!RegisterClassEx(&wndEx))
	{
		return 1;
	}

	HWND hWnd = CreateWindow(wndEx.lpszClassName, TEXT("Basic Thread Management"), WS_OVERLAPPEDWINDOW,
		200, 200, 840, 600, HWND_DESKTOP, NULL, wndEx.hInstance, NULL);

	HWND hRects[THREADS_NUMBER];
	hRects[0] = CreateWindow(wndEx.lpszClassName, L"...", WS_BORDER | WS_CHILD | WS_VISIBLE,
		20, 20, 180, 350, hWnd, NULL, hThis, NULL);
	hRects[1] = CreateWindow(wndEx.lpszClassName, L"...", WS_BORDER | WS_CHILD | WS_VISIBLE,
		220, 20, 180, 350, hWnd, NULL, hThis, NULL);
	hRects[2] = CreateWindow(wndEx.lpszClassName, L"...", WS_BORDER | WS_CHILD | WS_VISIBLE,
		420, 20, 180, 350, hWnd, NULL, hThis, NULL);
	hRects[3] = CreateWindow(wndEx.lpszClassName, L"...", WS_BORDER | WS_CHILD | WS_VISIBLE,
		620, 20, 180, 350, hWnd, NULL, hThis, NULL);
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);


	Thread threads[THREADS_NUMBER];
	for (int m = 0;m < THREADS_NUMBER;m++)
	{
		threads[m].Create(NULL, STATE_ASYNC | STATE_CONTINUOUS);//KEY STEP
		threads[m].SetUserData(hRects[m]);
	}

	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)threads);

	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass(wndEx.lpszClassName, wndEx.hInstance);

	return 0;



}



LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_CLOSE:
	{
		Thread* pThread = (Thread*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		pThread->Alert();
		for (int iIndex = 0; iIndex < THREADS_NUMBER; iIndex++)
		{
			pThread[iIndex].Join();
		}
		DestroyWindow(hWnd);
		break;
	}
	default:
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	}
	return 0;
}
