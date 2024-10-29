#include<Windows.h>

//#define MESSAGE_BOX

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{

#ifdef MESSAGE_BOX
	MessageBox(
		NULL,
		"Hello WinAPI!\nЭто Messagebox, детка.",
		NULL,
		MB_YESNOCANCEL | MB_ICONQUESTION | MB_HELP | MB_DEFBUTTON4 | MB_SYSTEMMODAL);
#endif // MESSAGE_BOX


	return 0;
}