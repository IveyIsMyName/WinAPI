#include <Windows.h>
#include "resource.h"

#define PLACEHOLDER_TEXT "Введите имя пользователя"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, NULL);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);
		//Устанавливаем текст приглашения в поле Login:
		SetDlgItemText(hwnd, IDC_EDIT_LOGIN, PLACEHOLDER_TEXT);
		//Сохраняем данные 'этого окна:
		HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
		SetWindowLongPtr(hEditLogin, GWLP_USERDATA, (LONG_PTR)TRUE);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EDIT_LOGIN:
			switch (HIWORD(wParam))
			{
			case EN_SETFOCUS:
			{
				HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
				BOOL PlaceholderActive = (BOOL)GetWindowLongPtr(hEditLogin, GWLP_USERDATA);
				//Если сейчас отображается текст приглашения, то очищаем поле и устанавливаем флаг в FALSE:
				if (PlaceholderActive)
				{
					SetDlgItemText(hwnd, IDC_EDIT_LOGIN, "");
					SetWindowLongPtr(hEditLogin, GWLP_USERDATA, (LONG_PTR)FALSE);
				}
			}
			break;
			case EN_KILLFOCUS:
			{
				HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
				CONST INT SIZE = 256;
				CHAR sz_text[SIZE] = {};
				GetWindowText(hEditLogin, sz_text, sizeof(sz_text));
				//Если пользователь оставил поле пустым, то устанавливаем текст приглашения и флаг в TRUE:
				if (strlen(sz_text) == 0)
				{
					SetDlgItemText(hwnd, IDC_EDIT_LOGIN, PLACEHOLDER_TEXT);
					SetWindowLongPtr(hEditLogin, GWLP_USERDATA, (LONG_PTR)TRUE);
				}
			}
			break;
			}
			break;
		case IDC_BUTTON_COPY:
		{
			HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
			HWND hEditPassword = GetDlgItem(hwnd, IDC_EDIT_PASSWORD);
			//Функция GetDlgItem() возвращает HWND дочернего элемента окна по его ResourceID.
			//HWND - Handler to Window (Дескриптор окна) - это число, при помощи которого можно обратиться к окну. 
			BOOL PlaceholderActive = (BOOL)GetWindowLongPtr(hEditLogin, GWLP_USERDATA);
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			GetWindowText(hEditLogin, sz_buffer, SIZE);
			if (!PlaceholderActive && strlen(sz_buffer) > 0)
			{
				SetWindowText(hEditPassword, sz_buffer);
			}
			else
			{
				SetWindowText(hEditPassword, "");
			}
			//SendMessage(hEditLogin, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);
			//SendMessage(hEditPassword, WM_SETTEXT, 0, (LPARAM)sz_buffer);
		}
		break;
		case IDOK:	   MessageBox(hwnd, "Была нажата кнопка ОК", "Info", MB_OK | MB_ICONINFORMATION); EndDialog(hwnd, 0); break;
		case IDCANCEL: EndDialog(hwnd, 0); break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}