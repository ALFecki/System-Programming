#ifndef UNICODE
#define UNICODE
#endif 

#define ID_FILE_CREATE 9001
#define ID_FILE_OPEN 9002
#define ID_FILE_SAVE 9003
#define ID_FONT_CHOICE 9004
#define ID_BG_CHOICE 9005

// ������� ��� ������������� ���������� ������� �� MenuBar'�

#include <windows.h>
#include <shobjidl.h> 
#include <iostream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
		HMENU hMenu, hFileSubMenu, hToolsSubMenu;

		// �������� MenuBar
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
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		// ��������� ��������� �� ������� ������ � ����
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
		// ������������ ���� ���������� ��������� ��������� � ������������� ��������� ����
		MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), false);
		return 0;
	}

	case WM_CTLCOLOREDIT:
	{
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, hEditFontColor);
		return (LRESULT)GetStockObject(DC_BRUSH);
	}


	case WM_CTLCOLORSTATIC:
	{
		HDC hdc = (HDC)wParam;
		HWND hwnd = (HWND)lParam;

		if (GetDlgCtrlID(hwnd) == GetDlgCtrlID(hWndEdit)) {
			SetBkColor(hdc, hEditBackgroundColor); // Set to red
			SetDCBrushColor(hdc, hEditBackgroundColor);
			(LRESULT)GetStockObject(DC_BRUSH); // return a DC brush.
		} else {
			DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		break;
	}

	case WM_CLOSE:
	{
		//if (MessageBox(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK) {
		//}
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
	if (ChooseColor(&chColor)) {
		hEditBackgroundColor = chColor.rgbResult;
		SendMessage(hWndEdit, WM_CTLCOLORSTATIC, NULL, TRUE);
	};

}