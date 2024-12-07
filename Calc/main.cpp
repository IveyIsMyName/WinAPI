#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include"resource.h"
#include <iostream>
#include <gdiplus.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment (lib, "Gdiplus.lib")
using namespace Gdiplus;

//HBRUSH hbrBkgnd;

//Image* bgImage = NULL;
CONST CHAR g_sz_CLASS_NAME[] = "Calc PV_319";

CONST INT g_i_BUTTON_SIZE = 50;
CONST INT g_i_INTERVAL = 5;
CONST INT g_i_BUTTON_DOUBLE_SIZE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;

CONST INT g_i_DISPLAY_WIDTH = g_i_BUTTON_SIZE * 5 + g_i_INTERVAL * 4;
CONST INT g_i_DISPLAY_HEIGHT = 48;

CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;
CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_DISPLAY_HEIGHT + g_i_INTERVAL;
CONST INT g_i_OPERATION_BUTTON_START_X = g_i_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3;
CONST INT g_i_OPERATION_BUTTON_START_Y = g_i_BUTTON_START_Y;
CONST INT g_i_CONTROL_BUTTON_START_X = g_i_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4;
CONST INT g_i_CONTROL_BUTTON_START_Y = g_i_BUTTON_START_Y;

CONST INT g_i_WINDOW_WIDTH = g_i_DISPLAY_WIDTH + g_i_START_X * 2 + 16;
CONST INT g_i_WINDOW_HEIGHT = g_i_DISPLAY_HEIGHT + g_i_START_Y * 2 + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4 + 38;

CONST CHAR* g_OPERATIONS[] = { "+", "-", "*", "/" };

CONST COLORREF g_DISPLAY_BACKGROUND[] = { RGB(0, 0, 100), RGB(0, 50, 0) };
CONST COLORREF g_DISPLAY_FOREGROUND[] = { RGB(255, 0, 0), RGB(0, 255, 0) };
CONST COLORREF g_WINDOW_BACKGROUND[] = { RGB(0, 0, 150), RGB(75, 75, 75) };

VOID SetSkin(HWND hwnd, CONST CHAR* skin);
VOID SetSkinFromDLL(HWND hwnd, CONST CHAR* skin);
VOID ProcessKey(HWND hwnd, WPARAM wParam, BOOL isKeyDown);
VOID GetExeDirectory(TCHAR* buffer, DWORD size);

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1) Регистрация класса окна:

	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	wClass.style = 0;
	wClass.cbSize = sizeof(wClass);
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;

	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wClass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	HBITMAP hBackground = (HBITMAP)LoadImage(hInstance, "Background calc.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	wClass.hbrBackground = CreatePatternBrush(hBackground);

	wClass.hInstance = hInstance;
	wClass.lpszClassName = g_sz_CLASS_NAME;
	wClass.lpfnWndProc = (WNDPROC)WndProc;
	wClass.lpszMenuName = NULL;

	// Переменные для инициализации GDI+
	//GdiplusStartupInput gdiplusStartupInput;
	//ULONG_PTR gdiplusToken;
	//GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if (!RegisterClassEx(&wClass))
	{
		MessageBox(NULL, "Class registration failed!", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}
	//2) Создание окна:
	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_CLASS_NAME,
		g_sz_CLASS_NAME,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,			//Position: X, Y
		g_i_WINDOW_WIDTH, g_i_WINDOW_HEIGHT,    //Size: Width, Height
		NULL,
		NULL,
		hInstance,
		NULL
	);

	//bgImage = Image::FromFile(L"Background calc.jpg"); // файл для бэкграунда

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//3) Запуск цикла сообщений:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static DOUBLE a = DBL_MIN;
	static DOUBLE b = DBL_MIN;
	static INT operation = 0;
	static BOOL input = FALSE;
	static BOOL input_operation = FALSE;

	static INT color_index = 0;
	static HANDLE hMyFont = NULL;

	switch (uMsg)
	{
	case WM_CREATE:
	{
		//AllocConsole();
		//freopen("CONOUT$", "w", stdout);
		HWND hEdit = CreateWindowEx
		(
			NULL, "Edit", "0",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
			10, 10,
			g_i_DISPLAY_WIDTH, g_i_DISPLAY_HEIGHT,
			hwnd,
			(HMENU)IDC_EDIT_DISPLAY,
			GetModuleHandle(NULL),
			NULL
		);
		/////////////		 FONT		////////////////////////
		//AddFontResourceEx("Fonts\\digital-7.ttf", FR_PRIVATE, 0);
		////////////////////////////////////////////////////////
		CHAR szExePath[MAX_PATH];
		GetExeDirectory(szExePath, MAX_PATH);
		CHAR szFontDllPath[MAX_PATH];
		PathCombine(szFontDllPath, szExePath, ("Fonts\\digital-7.dll"));

		HINSTANCE hInstFont = LoadLibrary(szFontDllPath); //("..\\Debug\\digital-7.dll");
		HRSRC hFontRes = FindResource(hInstFont, MAKEINTRESOURCE(101), "BINARY");
		HGLOBAL hFntMem = LoadResource(hInstFont, hFontRes);
		VOID* fntData = LockResource(hFntMem);
		DWORD nFonts = 0;
		DWORD len = SizeofResource(hInstFont, hFontRes);
		hMyFont = AddFontMemResourceEx(fntData, len, nullptr, &nFonts);
		HFONT hFont = CreateFont
		(
			g_i_DISPLAY_HEIGHT - 2, g_i_DISPLAY_HEIGHT / 3,
			0, 0, FW_BOLD,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			FF_DONTCARE, "Digital-7"
		);
		
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
		FreeLibrary(hInstFont);

		//TODO: Button Icons. 
		CHAR sz_digit[2] = "";
		for (int i = 6; i >= 0; i -= 3)
		{
			for (int j = 0; j < 3; j++)
			{
				sz_digit[0] = i + j + 49;  //49 - ASCII-код единицы
				CreateWindowEx
				(
					0, "Button", sz_digit,
					WS_VISIBLE | WS_CHILD | BS_BITMAP,
					g_i_BUTTON_START_X + j * (g_i_BUTTON_SIZE + g_i_INTERVAL),
					g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (2 - i / 3),
					g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
					hwnd,
					(HMENU)(IDC_BUTTON_1 + i + j),
					GetModuleHandle(NULL),
					NULL
				);
			}
		}
		CHAR filename[FILENAME_MAX]{};
		for (int i = 0; i < 10; i++)
		{
			sprintf(filename, "ButtonsBMP\\button_%i.bmp", i);
			HWND hButton = GetDlgItem(hwnd, IDC_BUTTON_0 + i);
			HBITMAP hBitmap = (HBITMAP)LoadImage
			(
				GetModuleHandle(NULL),
				filename, IMAGE_BITMAP,
				g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
				LR_LOADFROMFILE
			);
			SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);
		}
		HWND hButton0 = CreateWindowEx
		(
			0, "Button", "0",
			WS_CHILD | WS_VISIBLE | BS_BITMAP,
			g_i_BUTTON_START_X,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3,
			g_i_BUTTON_DOUBLE_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_0,
			GetModuleHandle(NULL),
			NULL
		);
		HBITMAP hBitmap0 = (HBITMAP)LoadImage
		(GetModuleHandle(NULL), "ButtonsBMP\\button_0.bmp",
			IMAGE_BITMAP, g_i_BUTTON_DOUBLE_SIZE, g_i_BUTTON_SIZE,
			LR_LOADFROMFILE);
		//SendMessage(hButton0, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap0);
		HWND hButtonPeriod = CreateWindowEx
		(
			NULL, "Button", ".",
			WS_CHILD | WS_VISIBLE | BS_BITMAP,
			g_i_BUTTON_START_X + g_i_BUTTON_DOUBLE_SIZE + g_i_INTERVAL,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_POINT,
			GetModuleHandle(NULL),
			NULL
		);
		HBITMAP hBitmapPeriod = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL), "ButtonsBMP\\button_point.bmp",
			IMAGE_BITMAP, g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			LR_LOADFROMFILE
		);
		//SendMessage(hButtonPeriod, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapPeriod);

		for (int i = 0; i < 4; i++)
		{
			CreateWindowEx
			(
				NULL, "Button", g_OPERATIONS[i],
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
				g_i_OPERATION_BUTTON_START_X,
				g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (3 - i),
				g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
				hwnd,
				(HMENU)(IDC_BUTTON_PLUS + i),
				GetModuleHandle(NULL),
				NULL
			);
		}
		/*for (int i = 0; i < 4; i++)
		{
			sprintf(filename, "ButtonsBMP\\button_op_%i.bmp", i);
			HWND hOperationButtons = GetDlgItem(hwnd, IDC_BUTTON_PLUS + i);
			HBITMAP hOperationBitmap = (HBITMAP)LoadImage
			(
				GetModuleHandle(NULL),
				filename, IMAGE_BITMAP,
				g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
				LR_LOADFROMFILE
			);
			SendMessage(hOperationButtons, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hOperationBitmap);
		}*/

		HWND hButtonBSP = CreateWindowEx
		(
			NULL, "Button", "<-",
			WS_CHILD | WS_VISIBLE | BS_BITMAP,
			g_i_CONTROL_BUTTON_START_X,
			g_i_CONTROL_BUTTON_START_Y,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_BSP,
			GetModuleHandle(NULL),
			NULL
		);
		HBITMAP hBitmapBSP = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL), "ButtonsBMP\\button_bsp.bmp",
			IMAGE_BITMAP, g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			LR_LOADFROMFILE
		);
		//SendMessage(hButtonBSP, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapBSP);
		HWND hButtonCLR = CreateWindowEx
		(
			NULL, "Button", "C",
			WS_CHILD | WS_VISIBLE | BS_BITMAP,
			g_i_CONTROL_BUTTON_START_X,
			g_i_CONTROL_BUTTON_START_Y + g_i_BUTTON_SIZE + g_i_INTERVAL,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_CLR,
			GetModuleHandle(NULL),
			NULL
		);
		HBITMAP hBitmapCLR = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL), "ButtonsBMP\\button_clr.bmp",
			IMAGE_BITMAP, g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			LR_LOADFROMFILE
		);
		//SendMessage(hButtonCLR, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapCLR);
		HWND hButtonEQL = CreateWindowEx
		(
			NULL, "Button", "=",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_CONTROL_BUTTON_START_X,
			g_i_CONTROL_BUTTON_START_Y + g_i_BUTTON_DOUBLE_SIZE + g_i_INTERVAL,
			g_i_BUTTON_SIZE, g_i_BUTTON_DOUBLE_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_EQUAL,
			GetModuleHandle(NULL),
			NULL
		);
		HBITMAP hBitmapEQL = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL), "ButtonsBMP\\button_equal.bmp",
			IMAGE_BITMAP, g_i_BUTTON_SIZE, g_i_BUTTON_DOUBLE_SIZE,
			LR_LOADFROMFILE
		);
		//SendMessage(hButtonEQL, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapEQL);

		//SetSkin(hwnd, "Square blue");
		SetSkinFromDLL(hwnd, "Square blue");
	}
	break;
	//case WM_DRAWITEM:
	//{
	//	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
	//	if (lpDrawItem->CtlType == ODT_BUTTON)
	//	{
	//		BOOL bPressed = (lpDrawItem->itemState & ODS_SELECTED);
	//		DrawEdge(lpDrawItem->hDC, &lpDrawItem->rcItem, /*bPressed ? EDGE_SUNKEN :*/ EDGE_RAISED, BF_RECT);
	//		SetTextColor(lpDrawItem->hDC, RGB(0, 0, 0));
	//		// Устанавливаем прозрачный режим фона
	//		SetBkMode(lpDrawItem->hDC, TRANSPARENT);
	//		// Загружаем фон окна
	//		Graphics graphics(lpDrawItem->hDC);
	//		graphics.DrawImage(
	//			bgImage,
	//			static_cast<REAL>(lpDrawItem->rcItem.left),
	//			static_cast<REAL>(lpDrawItem->rcItem.top),
	//			static_cast<REAL>(lpDrawItem->rcItem.right - lpDrawItem->rcItem.left),
	//			static_cast<REAL>(lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top)
	//		);

	//		// Рисование текста кнопки
	//		char szText[256];
	//		GetWindowText(lpDrawItem->hwndItem, szText, sizeof(szText));
	//		DrawText(lpDrawItem->hDC, szText, -1, &lpDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	//		// Устанавливаем карандаш для рисования границ
	//		HDC hdc = lpDrawItem->hDC;
	//		HPEN hPen = CreatePen(PS_SOLID, 2, RGB(169, 169, 169));
	//		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

	//		// Устанавливаем кисть для закраски (NULL, чтобы закраски не было)
	//		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

	//		// Рисуем прямоугольник (границы кнопки)
	//		Rectangle(hdc, lpDrawItem->rcItem.left, lpDrawItem->rcItem.top, lpDrawItem->rcItem.right, lpDrawItem->rcItem.bottom);

	//		// Восстанавливаем старые GDI-объекты
	//		SelectObject(hdc, hOldPen);
	//		SelectObject(hdc, hOldBrush);

	//		// Уничтожаем созданные объекты
	//		DeleteObject(hPen);

	//		return TRUE; // Обработано
	//	}
	//	break;
	//}
	//break;
	case WM_CTLCOLOREDIT:
	{
		HDC hdcEdit = (HDC)wParam;
		HWND hwndEdit = (HWND)lParam;

		// Проверяем, что это наш Edit control по его ID
		if (GetDlgCtrlID(hwndEdit) == IDC_EDIT_DISPLAY)
		{
			// Устанавливаем цвет текста
			SetTextColor(hdcEdit, g_DISPLAY_FOREGROUND[color_index]);

			// Устанавливаем цвет фона
			SetBkColor(hdcEdit, g_DISPLAY_BACKGROUND[color_index]);
		HBRUSH hbrBackground = CreateSolidBrush(g_WINDOW_BACKGROUND[color_index]);
		SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)hbrBackground);
		SendMessage(hwnd, WM_ERASEBKGND, wParam, 0);
		// Возвращаем кисть для заливки фона
		return (INT_PTR)hbrBackground;
		}
	}
	break;
	//case WM_PAINT:
	//{
	//	/*PAINTSTRUCT ps;
	//	HDC hdc = BeginPaint(hwnd, &ps);
	//	Graphics graphics(hdc);

	//	RECT rect;
	//	GetClientRect(hwnd, &rect);
	//	graphics.DrawImage(bgImage, 0, 0, rect.right, rect.bottom);
	//	EndPaint(hwnd, &ps);*/
	//}
	//break;
	case WM_COMMAND:
	{
		CONST INT SIZE = 256;
		CHAR sz_display[SIZE] = {};
		CHAR sz_digit[2] = {};
		HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
		SendMessage(hEditDisplay, WM_GETTEXT, SIZE, (LPARAM)sz_display);
		if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_9)
		{
			//TODO: Если первый символ '.', то последующий ввод неверный
			if (!input && !input_operation)
			{
				SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_CLR), 0);
				sz_display[0] = 0;
			}
			if (!input && input_operation)sz_display[0] = 0;
			sz_digit[0] = LOWORD(wParam) - IDC_BUTTON_0 + '0';
			if (strlen(sz_display) == 1 && sz_display[0] == '0')
				sz_display[0] = sz_digit[0];
			else
				strcat(sz_display, sz_digit);
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
			input = TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_POINT)
		{
			if (strchr(sz_display, '.') && input)break;
			if (input_operation && a == atof(sz_display))strcpy(sz_display, "0.");
			else strcat(sz_display, ".");
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
			input = TRUE;
			strcat(sz_display, ".");
		}
		if (LOWORD(wParam) == IDC_BUTTON_BSP)
		{
			if (strlen(sz_display) == 1)sz_display[0] = '0';
			else sz_display[strlen(sz_display) - 1] = 0;
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
		}
		if (LOWORD(wParam) == IDC_BUTTON_CLR)
		{
			a = b = DBL_MIN;
			operation = 0;
			input = FALSE;
			input_operation = FALSE;
			//ZeroMemory(sz_display, SIZE);
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)"0");
		}
		if (LOWORD(wParam) >= IDC_BUTTON_PLUS && LOWORD(wParam) <= IDC_BUTTON_SLASH)
		{
			if (a == DBL_MIN)a = atof(sz_display);
			input = FALSE;
			if (input_operation)SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_EQUAL), 0);
			operation = LOWORD(wParam);
			input = FALSE;
			input_operation = TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_EQUAL)
		{
			if (input || b == DBL_MIN && !input)b = atof(sz_display);
			input = FALSE;
			switch (operation)
			{
			case IDC_BUTTON_PLUS:
				a += b; break;
			case IDC_BUTTON_MINUS:
				a -= b; break;
			case IDC_BUTTON_ASTER:
				a *= b; break;
			case IDC_BUTTON_SLASH:
				a /= b; break;
			}
			input_operation = FALSE;
			if (a == DBL_MIN)strcpy(sz_display, "0");
			else sprintf(sz_display, "%g", a);
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
		}
		SetFocus(hwnd);
	}
	break;
	case WM_KEYDOWN:
	{
		ProcessKey(hwnd, wParam, TRUE);
	}
	break;
	case WM_KEYUP:
	{
		ProcessKey(hwnd, wParam, FALSE);
	}
	break;
	case WM_CONTEXTMENU:
	{
		CHAR sz_buffer[FILENAME_MAX] = "";
		HMENU hMainMenu = CreatePopupMenu();
		HMENU hSubMenu = CreatePopupMenu();
		InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hSubMenu, "Skins");
		InsertMenu(hSubMenu, 0, MF_BYPOSITION | MF_STRING, IDR_SQUARE_BLUE, "Square blue");
		InsertMenu(hSubMenu, 1, MF_BYPOSITION | MF_STRING, IDR_METAL_MISTRAL, "Metal mistral");
		InsertMenu(hSubMenu, 2, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		InsertMenu(hSubMenu, 3, MF_BYPOSITION | MF_STRING, IDR_EXIT, "Exit");
		BOOL skin_index = TrackPopupMenuEx(hMainMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_RETURNCMD, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
		switch (skin_index)
		{
		case IDR_SQUARE_BLUE:	strcpy(sz_buffer, "Square blue");  break;
		case IDR_METAL_MISTRAL:	strcpy(sz_buffer, "Metal mistral"); break;
		case IDR_EXIT:			DestroyWindow(hwnd);
								PostQuitMessage(0);
		}

		//if (skin_index)SetSkin(hwnd, sz_buffer);
		if (skin_index)SetSkinFromDLL(hwnd, sz_buffer);

		DestroyMenu(hSubMenu);
		DestroyMenu(hMainMenu);

		if (skin_index >= IDR_SQUARE_BLUE && skin_index <= IDR_METAL_MISTRAL)
		{
		color_index = skin_index - IDR_CONTEXT_MENU - 1;
		HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
		HDC hdcDisplay = GetDC(hEditDisplay);
		SendMessage(hwnd, WM_CTLCOLOREDIT, (WPARAM)hdcDisplay, (LPARAM)hEditDisplay);
		ReleaseDC(hEditDisplay, hdcDisplay);
		SendMessage(hEditDisplay, WM_GETTEXT, MAX_PATH, (LPARAM)sz_buffer);
		SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_buffer);
		}
	}
	break;
	case WM_DESTROY:
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
		HDC hdc = GetDC(hEdit);
		ReleaseDC(hEdit, hdc);
		RemoveFontMemResourceEx(hMyFont);
		//if (bgImage)delete bgImage;
		PostQuitMessage(0);
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	default:	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}

CONST CHAR* g_BUTTON_FILENAME[] =
{
"button_0",
"button_1",
"button_2",
"button_3",
"button_4",
"button_5",
"button_6",
"button_7",
"button_8",
"button_9",
"button_point",
"button_plus",
"button_minus",
"button_aster",
"button_slash",
"button_bsp",
"button_clr",
"button_equal",
};

VOID ProcessKey(HWND hwnd, WPARAM wParam, BOOL isKeyDown)
{
	INT vk = (INT)wParam;
	INT buttonID = 0;

	if (GetKeyState(VK_SHIFT) < 0)
	{
		if (vk == '8')
		{
			buttonID = IDC_BUTTON_ASTER;
		}
	}
	else if ((vk >= '0' && vk <= '9') || (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9))
	{
		if (vk >= '0' && vk <= '9')
			buttonID = vk - '0' + IDC_BUTTON_0;
		else
			buttonID = vk - VK_NUMPAD0 + IDC_BUTTON_0;
	}
	else
	{
		switch (vk)
		{
		case VK_DECIMAL:
		case VK_OEM_PERIOD:
			buttonID = IDC_BUTTON_POINT;
			break;
		case VK_ESCAPE:
			buttonID = IDC_BUTTON_CLR;
			break;
		case VK_BACK:
			buttonID = IDC_BUTTON_BSP;
			break;
		case VK_RETURN:
			buttonID = IDC_BUTTON_EQUAL;
			break;
		case VK_OEM_PLUS:
		case VK_ADD:
			buttonID = IDC_BUTTON_PLUS;
			break;
		case VK_OEM_MINUS:
		case VK_SUBTRACT:
			buttonID = IDC_BUTTON_MINUS;
			break;
		case VK_OEM_2:
		case VK_DIVIDE:
			buttonID = IDC_BUTTON_SLASH;
			break;
		case VK_MULTIPLY:
			buttonID = IDC_BUTTON_ASTER;
			break;
		}
	}
	if (buttonID != 0)
	{
		HWND hButton = GetDlgItem(hwnd, buttonID);
		SendMessage(hButton, BM_SETSTATE, isKeyDown, 0);
	}
	if (!isKeyDown)
	{
		SendMessage(hwnd, WM_COMMAND, buttonID, 0);
	}
}

VOID SetSkin(HWND hwnd, CONST CHAR* skin)
{
	//CHAR sz_path[MAX_PATH]{};
	CHAR sz_filename[FILENAME_MAX]{};
	for (int i = 0; i < 18; i++)
	{
		HWND hButton = GetDlgItem(hwnd, IDC_BUTTON_0 + i);
		sprintf(sz_filename, "ButtonsBMP\\%s\\%s.bmp", skin, g_BUTTON_FILENAME[i]);
		HBITMAP bmpButton = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL),
			sz_filename,
			IMAGE_BITMAP,
			i + IDC_BUTTON_0 == IDC_BUTTON_0 ? g_i_BUTTON_DOUBLE_SIZE : g_i_BUTTON_SIZE,
			i + IDC_BUTTON_0 == IDC_BUTTON_EQUAL ? g_i_BUTTON_DOUBLE_SIZE : g_i_BUTTON_SIZE,
			LR_LOADFROMFILE
		);
		SendMessage(hButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpButton);
	}
}

VOID SetSkinFromDLL(HWND hwnd, CONST CHAR* skin)
{
	CHAR szExePath[MAX_PATH]{};
	GetExeDirectory(szExePath, MAX_PATH);
	CHAR filename[MAX_PATH]{};
	sprintf_s(filename, MAX_PATH, "%s\\ButtonsBMP\\%s", szExePath, skin);
	HMODULE hInst = LoadLibrary(filename);
	for (int i = IDC_BUTTON_0; i <= IDC_BUTTON_EQUAL; i++)
	{
		HBITMAP buttonBMP = (HBITMAP)LoadImage
		(
			hInst,
			MAKEINTRESOURCE(i),
			IMAGE_BITMAP,
			i > IDC_BUTTON_0 ? g_i_BUTTON_SIZE : g_i_BUTTON_DOUBLE_SIZE,
			i < IDC_BUTTON_EQUAL ? g_i_BUTTON_SIZE : g_i_BUTTON_DOUBLE_SIZE,
			NULL
		);
		SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)buttonBMP);
	}
	FreeLibrary(hInst);
}

VOID GetExeDirectory(TCHAR* buffer, DWORD size)
{
	GetModuleFileName(NULL, buffer, size);
	PathRemoveFileSpec(buffer);
}