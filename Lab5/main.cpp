#include <Windows.h>
#include <CommCtrl.h>
#include <string>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void EnumerateRegistryKeys(HWND hTreeView, HKEY hKey, HTREEITEM hParentItem);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0,
					  hInstance, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
					  (HBRUSH)(COLOR_WINDOW + 1), NULL, L"TreeViewWindowClass", LoadIcon(NULL, IDI_APPLICATION) };

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Failed to register window class.", L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowEx(0, L"TreeViewWindowClass", L"Registry Folders",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600, NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) {
		MessageBox(NULL, L"Failed to create window.", L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	HWND hTreeView = CreateWindowEx(0, WC_TREEVIEW, L"", WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS,
		10, 10, 760, 540, hwnd, NULL, hInstance, NULL);

	HKEY hKey;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"", 0, KEY_READ, &hKey);

	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = TVI_ROOT;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
	tvInsert.item.pszText = (LPWSTR)L"CurrentVersion";
	tvInsert.item.lParam = (LPARAM)hKey;
	HTREEITEM hRootItem = (HTREEITEM)SendMessage(hTreeView, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);

	EnumerateRegistryKeys(hTreeView, hKey, hRootItem);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void EnumerateRegistryKeys(HWND hTreeView, HKEY hKey, HTREEITEM hParentItem) {
	wchar_t szName[MAX_PATH];
	DWORD dwIndex = 0;
	DWORD cbName = MAX_PATH;

	while (RegEnumKeyEx(hKey, dwIndex, szName, &cbName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
		TVINSERTSTRUCT tvInsert;
		tvInsert.hParent = hParentItem;
		tvInsert.hInsertAfter = TVI_LAST;
		tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
		tvInsert.item.pszText = szName;
		tvInsert.item.lParam = (LPARAM)hKey;
		HTREEITEM hItem = (HTREEITEM)SendMessage(hTreeView, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);

		HKEY hSubKey;
		if (RegOpenKeyEx((HKEY)tvInsert.item.lParam, szName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
			EnumerateRegistryKeys(hTreeView, hSubKey, hItem);
			RegCloseKey(hSubKey);
		}

		dwIndex++;
		cbName = MAX_PATH;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}