#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include<stdio.h>
#include"resource.h"

CONST CHAR* g_VALUES[] = { "This", "is", "my", "first", "List", "Box" };

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCdmShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hList = GetDlgItem(hwnd, IDC_LIST);
		for (int i = 0; i < sizeof(g_VALUES) / sizeof(g_VALUES[0]); i++)
		{
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)g_VALUES[i]);
		}
		SendMessage(hList, LB_SETCURSEL, 0, 0);
		HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE]{};
			HWND hList = GetDlgItem(hwnd, IDC_LIST);
			INT i = SendMessage(hList, LB_GETCURSEL, 0, 0);
			SendMessage(hList, LB_GETTEXT, i, (LPARAM)sz_buffer);
			CHAR sz_message[SIZE]{};
			sprintf(sz_message, "Вы выбрали пункт №%i со значением\"%s\".", i, sz_buffer);
			MessageBox(hwnd, sz_message, "Info", MB_OK | MB_ICONINFORMATION);
		}
		break;
		case IDCANCEL: EndDialog(hwnd, 0);
			break;
		case IDC_ADD:
		{
			HWND hList = GetDlgItem(hwnd, IDC_LIST);
			HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
			CONST INT SIZE = 256;
			CHAR sz_input[SIZE]{};
			GetWindowTextA(hEdit, sz_input, SIZE);
			if (strlen(sz_input) > 0)
			{
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)sz_input);
				SetWindowText(hEdit, "");
			}
			else
			{
				MessageBoxA(hwnd, "Введите строку для добавления.", "Error", MB_OK | MB_ICONERROR);
			}
		}
		break;
		case IDC_REMOVE:
		{
			HWND hList = GetDlgItem(hwnd, IDC_LIST);
			INT i = SendMessage(hList, LB_GETCURSEL, 0, 0);
			SendMessage(hList, LB_DELETESTRING, i, 0);
		}
		break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}