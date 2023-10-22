#ifndef UNICODE
#define UNICODE
#endif 

#define ID_FILE_CREATE 9001
#define ID_FILE_OPEN 9002
#define ID_FILE_SAVE 9003
#define VK_A 0x41
#define ID_FONT_CHOICE 9004
#define ID_BG_CHOICE 9005

// Макросы для распознавания конкретной команды из MenuBar'а

#include <windows.h>
#include <shobjidl.h> 
#include <iostream>
#include <optional>
#include <utility>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HotkeyProc(int nCode, WPARAM wParam, LPARAM lParam);

std::optional<std::tuple<HANDLE, HANDLE, LPVOID>> InitializeMapping(PWSTR pszFilePath, DWORD openFlag, DWORD dwFileSize = NULL);
void UnitializeMapping(HANDLE hFile, HANDLE hMapFile, LPVOID lpData);
void OpenFile(HWND hwnd);
void SaveFile(HWND hwnd);

void FontChoice(HWND hwnd);
void BackgroundColor(HWND hwnd);

HWND hWndEdit = NULL;
COLORREF hEditFontColor = RGB(0, 0, 0);
HFONT hEditFont = NULL;
COLORREF hEditBackgroundColor = RGB(255, 255, 255);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { };

	HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, HotkeyProc, NULL, 0);

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Notepad--",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL) {
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(hHook);
	return 0;
}

UINT_PTR CALLBACK Lpcchookproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_INITDIALOG) {
		SetWindowPos(hWnd, HWND_TOPMOST, 400, 400, 0, 0, SWP_NOSIZE);
	}
	return 0;
}


LRESULT CALLBACK HotkeyProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0)
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	if (wParam == WM_KEYDOWN) {
		if (nCode >= 0) {
			KBDLLHOOKSTRUCT* pKeyboardHook = (KBDLLHOOKSTRUCT*)lParam;
			if (pKeyboardHook->vkCode == 'S' && GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_SHIFT) & 0x8000) {
				MessageBox(NULL, L"Вы нажали Ctrl + Shift + S!", L"Горячая клавиша", MB_OK);
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CREATE:
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (!SUCCEEDED(hr)) {
			DestroyWindow(hwnd);
			return 0;
		}
		HMENU hMenu, hFileSubMenu, hToolsSubMenu;

		hMenu = CreateMenu();
		hFileSubMenu = CreatePopupMenu();
		hToolsSubMenu = CreatePopupMenu();
		AppendMenu(hFileSubMenu, MF_STRING, ID_FILE_CREATE, L"&Create");
		AppendMenu(hFileSubMenu, MF_STRING, ID_FILE_OPEN, L"&Open");
		AppendMenu(hFileSubMenu, MF_STRING, ID_FILE_SAVE, L"&Save");
		AppendMenu(hMenu, MF_POPUP, (UINT)hFileSubMenu, L"&File");
		AppendMenu(hToolsSubMenu, MF_STRING, ID_FONT_CHOICE, L"&Font");
		AppendMenu(hToolsSubMenu, MF_STRING, ID_BG_CHOICE, L"&Background");
		AppendMenu(hMenu, MF_POPUP, (UINT)hToolsSubMenu, L"&Tools");

		SetMenu(hwnd, hMenu);

		hWndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), NULL,
			WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN | WS_VISIBLE,
			0, 0, 780, 560, hwnd, NULL,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);

		hEditFont = (HFONT)SendMessage(hWndEdit, WM_GETFONT, 0, 0);
		RegisterHotKey(hwnd, 1, MOD_CONTROL, VK_A);
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case ID_FILE_CREATE:
			SetWindowTextA(hWndEdit, "");
			break;

		case ID_FILE_OPEN:
			OpenFile(hwnd);
			break;

		case ID_FILE_SAVE:
			SaveFile(hwnd);
			break;

		case ID_FONT_CHOICE:
			FontChoice(hwnd);
			break;

		case ID_BG_CHOICE:
			BackgroundColor(hwnd);
			break;
		default:
			break;
		}
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), false);
		return 0;
	}

	case WM_CTLCOLOREDIT:
	{
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, hEditFontColor);
		SetDCBrushColor(hdc, hEditBackgroundColor);
		SetBkColor(hdc, hEditBackgroundColor);
		return (LRESULT)GetStockObject(DC_BRUSH);
	}

	case WM_HOTKEY:
	{
		if (wParam == 1) {
			SendMessage(hWndEdit, EM_SETSEL, 0, GetWindowTextLength(hWndEdit));
		}
		break;
	}

	case WM_CLOSE:
	{
		CoUninitialize();
		DestroyWindow(hwnd);
		return 0;
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void OpenFile(HWND hwnd) {
	IFileOpenDialog* pFileOpen;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

	if (!SUCCEEDED(hr)) {
		return;
	}

	hr = pFileOpen->Show(NULL);
	if (!SUCCEEDED(hr)) {
		return;
	}
	IShellItem* pItem;

	hr = pFileOpen->GetResult(&pItem);
	if (!SUCCEEDED(hr)) {
		pItem->Release();
		pFileOpen->Release();
		return;
	}
	PWSTR pszFilePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

	if (!SUCCEEDED(hr)) {
		pItem->Release();
		pFileOpen->Release();
		return;
	}
	auto resultInitialize = InitializeMapping(pszFilePath, OPEN_EXISTING);

	if (!resultInitialize) {
		return;
	}
	auto [hFile, hMapFile, lpData] = *resultInitialize;
	DWORD fileSize = GetFileSize(hFile, NULL);
	LPSTR buffer = (LPSTR)GlobalAlloc(GPTR, fileSize + 1);

	if (memcpy((CHAR*)buffer, lpData, fileSize)) {
		SetWindowTextA(hWndEdit, buffer);
	} else {
		MessageBoxA(hwnd, "Cannot read file", "Error", MB_OK);
	}
	GlobalFree((HGLOBAL)buffer);
	ShowWindow(hWndEdit, SW_SHOW);
	SetFocus(hWndEdit);
	UnitializeMapping(hFile, hMapFile, lpData);
}


void SaveFile(HWND hwnd) {
	IFileSaveDialog* pFileSave;
	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
		IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
	if (!SUCCEEDED(hr)) {
		MessageBoxA(hwnd, "File dialog open failed", "Failed", MB_ICONERROR);
		return;
	}
	hr = pFileSave->Show(NULL);
	if (!SUCCEEDED(hr)) {
		MessageBoxA(hwnd, "File dialog open show failed", "Failed", MB_ICONERROR);
		return;
	}
	IShellItem* pItem;
	hr = pFileSave->GetResult(&pItem);
	if (!SUCCEEDED(hr)) {
		MessageBoxA(hwnd, "Failed to get result from file dialog", "Failed", MB_ICONERROR);
		return;
	}

	PWSTR pszFilePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (!SUCCEEDED(hr)) {
		MessageBoxA(hwnd, "Failed to get file path", "Failed", MB_ICONERROR);
		return;
	}
	DWORD dwFileSize = GetWindowTextLength(hWndEdit) + 1;
	auto resultInitialize = InitializeMapping(pszFilePath, CREATE_ALWAYS, dwFileSize);
	if (!resultInitialize) {
		return;
	}
	auto [hFile, hMapFile, lpData] = *resultInitialize;

	LPSTR buffer = (LPSTR)GlobalAlloc(GPTR, dwFileSize);;
	GetWindowTextA(hWndEdit, buffer, dwFileSize);

	if (memcpy((CHAR*)lpData, buffer, dwFileSize)) {
		MessageBoxA(hwnd, "File successfully saved", "Saved", MB_OK);
	} else {
		MessageBoxA(hwnd, "File save failed", "Failed", MB_ICONERROR);
	}
	UnitializeMapping(hFile, hMapFile, lpData);
	GlobalFree((HGLOBAL)buffer);
}


std::optional<std::tuple<HANDLE, HANDLE, LPVOID>> InitializeMapping(PWSTR pszFilePath, DWORD openFlag, DWORD dwFileSize) {
	HANDLE hFile = CreateFile(pszFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, openFlag, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBoxA(NULL, "Error creating file", "Error", MB_ICONERROR);
		return {};
	}
	if (dwFileSize == NULL) {
		dwFileSize = GetFileSize(hFile, NULL);
	}
	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwFileSize, NULL);

	if (hMapFile == NULL) {
		MessageBoxA(NULL, "Error creating mapping", "Error", MB_ICONERROR);
		UnitializeMapping(hFile, INVALID_HANDLE_VALUE, NULL);
		return {};
	}
	LPVOID lpData = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, dwFileSize);
	if (lpData == NULL) {
		MessageBoxA(NULL, "Error creating mapping view", "Error", MB_ICONERROR);
		UnitializeMapping(hFile, hMapFile, NULL);
		return {};
	}
	return { { hFile, hMapFile,  lpData} };
}

void UnitializeMapping(HANDLE hFile, HANDLE hMapFile, LPVOID lpData) {
	if (lpData != NULL) {
		UnmapViewOfFile(lpData);
	}
	if (hMapFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hMapFile);
	}
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}
}


void FontChoice(HWND hwnd) {
	CHOOSEFONT cf = { sizeof(CHOOSEFONT) };
	LOGFONT lf;
	GetObject(hEditFont, sizeof(LOGFONT), &lf);
	cf.Flags = CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lf;

	if (!ChooseFont(&cf))
		return;
	HFONT hf = CreateFontIndirect(&lf);
	hEditFontColor = cf.rgbColors;
	if (hf) {
		SendMessage(hWndEdit, WM_SETFONT, (WPARAM)hf, TRUE);
		SendMessage(hWndEdit, WM_CTLCOLOREDIT, (WPARAM)hf, TRUE);
	}
}

void BackgroundColor(HWND hwnd) {
	CHOOSECOLOR chColor;
	COLORREF acrCustClr[16];
	chColor.lStructSize = sizeof(chColor);
	chColor.lpCustColors = (LPDWORD)acrCustClr;
	chColor.rgbResult = hEditBackgroundColor;
	chColor.Flags = CC_FULLOPEN | CC_RGBINIT;
	chColor.hwndOwner = NULL;
	chColor.lpfnHook = Lpcchookproc;
	if (ChooseColor(&chColor)) {
		hEditBackgroundColor = chColor.rgbResult;
		HBRUSH hBrush = CreateSolidBrush(hEditBackgroundColor);
		SetClassLongPtr(hWndEdit, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(hBrush));
		InvalidateRect(hWndEdit, nullptr, TRUE);
	};

}