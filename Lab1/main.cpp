#ifndef UNICODE
#define UNICODE
#endif 

#define ID_FILE_CREATE 9001
#define ID_FILE_OPEN 9002
#define ID_FILE_SAVE 9003
#define VK_A 0x41

// Макросы для распознавания конкретной команды из MenuBar'а

#include <windows.h>
#include <shobjidl.h> 
#include <iostream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK HotKeyProc(int nCode, WPARAM wParam, LPARAM lParam);

void OpenFile(HWND hwnd);
void SaveFile(HWND hwnd);

HWND hWndEdit = NULL;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { };

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
	return 0;
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
		HMENU hMenu, hSubMenu;

		// Создание MenuBar
		hMenu = CreateMenu();
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_CREATE, L"&Create");
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, L"&Open");
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVE, L"&Save");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"&File");
		SetMenu(hwnd, hMenu);
		hWndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), NULL,
			WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN | WS_VISIBLE,
			0, 0, 780, 560, hwnd, NULL,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);
		RegisterHotKey(hwnd, 1, MOD_CONTROL, VK_A);
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
			// Обработка сообщений из нажатых кнопок в меню
		case ID_FILE_CREATE:
			SetWindowTextA(hWndEdit, "");
			break;

		case ID_FILE_OPEN:
			OpenFile(hwnd);
			break;

		case ID_FILE_SAVE:
			SaveFile(hwnd);
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
		// Растягивание окна текстового редактора совместно с растягиванием основного окна
		MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), false);
		return 0;
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

	if (SUCCEEDED(hr)) {
		hr = pFileOpen->Show(NULL);

		if (SUCCEEDED(hr)) {
			IShellItem* pItem;
			hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr)) {
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

				if (SUCCEEDED(hr)) {
					HANDLE hFile = CreateFile(pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
					DWORD fileSize = GetFileSize(hFile, NULL);

					LPSTR buffer = (LPSTR)GlobalAlloc(GPTR, fileSize + 1);
					DWORD read;

					if (ReadFile(hFile, buffer, fileSize, &read, NULL)) {
						SetWindowTextA(hWndEdit, buffer);
					} else {
						MessageBoxA(hwnd, "Cannot read file", "Error", MB_OK);
					}
					GlobalFree((HGLOBAL)buffer);
					ShowWindow(hWndEdit, SW_SHOW);
					SetFocus(hWndEdit);
					CloseHandle(hFile);
				}
				pItem->Release();
			}
		}
		pFileOpen->Release();
	}
}


void SaveFile(HWND hwnd) {
	IFileSaveDialog* pFileSave;
	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
		IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
	if (SUCCEEDED(hr)) {
		hr = pFileSave->Show(NULL);
		if (SUCCEEDED(hr)) {
			IShellItem* pItem;
			hr = pFileSave->GetResult(&pItem);
			if (SUCCEEDED(hr)) {
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				if (SUCCEEDED(hr)) {
					HANDLE hFile = CreateFile(pszFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
					DWORD fileSize = GetWindowTextLength(hWndEdit) + 1;
					LPSTR buffer = (LPSTR)GlobalAlloc(GPTR, fileSize);;
					GetWindowTextA(hWndEdit, buffer, fileSize);
					DWORD wroted;

					if (WriteFile(hFile, (void*)buffer, fileSize, &wroted, NULL)) {
						MessageBox(hwnd, L"File successfully saved", L"Saved", MB_OK);
						CloseHandle(hFile);
					}
					GlobalFree((HGLOBAL)buffer);
				}
			}

		}
	}

}