#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include <stdio.h>
#include"resource.h"

CONST CHAR g_sz_MY_WINDOW_CLASS[] = "My Window";  //Имя класса окна

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1)Регистрация класса окна:
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));

	wc.style = 0;
	wc.cbSize = sizeof(wc);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	//cb... - Count Bytes

	//wc.hIcon = (HICON)LoadImage(hInstance, "Ico\\bitcoin.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	//wc.hIconSm = (HICON)LoadImage(hInstance, "Ico\\emotion.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = (HCURSOR)LoadImage(hInstance, "normal select.ani", IMAGE_CURSOR, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	//wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hInstance = hInstance;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_sz_MY_WINDOW_CLASS;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Class registration failed", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}

	//2) Создание окна:
	INT screen_width = GetSystemMetrics(SM_CXSCREEN);
	INT screen_height = GetSystemMetrics(SM_CYSCREEN);
	INT window_width = screen_width * 3 / 4;
	INT window_height = screen_height * 3 / 4;
	INT window_start_x = (screen_width - window_width) / 2;
	INT window_start_y = (screen_height - window_height) / 2;

	HWND hwnd = CreateWindowEx
	(
		NULL,	//ExStyles
		g_sz_MY_WINDOW_CLASS,	//Class name
		g_sz_MY_WINDOW_CLASS,	//Window title
		WS_OVERLAPPEDWINDOW,   //Window style
		window_start_x, window_start_y,
		window_width, window_height,
		//CW_USEDEFAULT, CW_USEDEFAULT, // Window position
		//CW_USEDEFAULT, CW_USEDEFAULT, // Window size
		NULL,		//Parent window
		NULL,		//Main menu RecourceID for MainWindow or ResourceID for ChildWindow
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow); //Задает режим отображения окна (Развернуто на весь экран, свернуто в окно, свернуто в панель задач)
	UpdateWindow(hwnd);			//Прорисовывает окно

	//3) Запуск цикла сообщений:
	MSG msg;	//Создаем сообщение
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.message;
}

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);
	}
	break;
	case WM_CREATE:
		break;
	case WM_SIZE:
	case WM_MOVE:
	{
		//int x_pos = (int)(short)LOWORD(lParam);
		//int y_pos = (int)(short)HIWORD(lParam);
		RECT rect;
		GetWindowRect(hwnd, &rect);
		int x_pos = rect.left;
		int y_pos = rect.top;
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		char sz_title[256];
		sprintf(sz_title, "My window position (%d, %d) | Size (%d x %d)", x_pos, y_pos, width, height);
		SetWindowText(hwnd, sz_title);
	}
		break;
	case WM_COMMAND:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	default:	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}