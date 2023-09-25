#ifndef UNICODE
#define UNICODE
#endif 

#define ID_FILE_OPEN 9001
#define ID_FILE_SAVE 9002

#include <windows.h>
#include <shobjidl.h> 
#include <iostream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND hWndEdit = NULL;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL) {
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	// Run the message loop.

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
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);
		if (!SUCCEEDED(hr)) {
			DestroyWindow(hwnd);
			return 0;
		}
		HMENU hMenu, hSubMenu;

		hMenu = CreateMenu();
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, L"&Open");
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVE, L"&Save");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"&File");
		SetMenu(hwnd, hMenu);
		hWndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), NULL,
			WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN,
			0, 0, 780, 560, hwnd, NULL,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case ID_FILE_OPEN:
		{
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
							size_t outSize;
							

							if (ReadFile(hFile, buffer, fileSize, &read, NULL)) {
								wchar_t* convertedBuffer = new wchar_t[read + 1];
								mbstowcs_s(&outSize, convertedBuffer, read + 1, buffer, read);
								SetWindowTextW(hWndEdit, convertedBuffer);
							}
							else {
								MessageBox(hwnd, L"Cannot read file", L"Error", MB_OK);
							}

							ShowWindow(hWndEdit, SW_SHOW);
							SetFocus(hWndEdit);
							CloseHandle(hFile);
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			break;
		}

		case ID_FILE_SAVE:
		{
			IFileSaveDialog* pFileSave;
			HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
				IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
			if (SUCCEEDED(hr)) {
				hr = pFileSave->Show(NULL);
			}
		}

		default:
			break;
		}
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), false);
		return 0;
	}

	case WM_CLOSE:
	{
		if (MessageBox(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK) {
			CoUninitialize();
			DestroyWindow(hwnd);
		}
		// Else: User canceled. Do nothing.
		return 0;
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
