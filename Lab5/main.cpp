#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

#define IDC_EDIT_VALUE 9001

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditValueDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void EnumerateRegistryKeys(HWND hTreeView, HKEY hKey, HTREEITEM hParentItem);

HWND hTreeView = NULL;

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

	hTreeView = CreateWindowEx(0, WC_TREEVIEW, L"", WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS,
		10, 10, 760, 540, hwnd, NULL, hInstance, NULL);

	HKEY hKey;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"", 0, KEY_READ, &hKey);

	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = TVI_ROOT;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
	tvInsert.item.pszText = (LPWSTR)L"HKEY_LOCAL_MACHINE";
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

void EditRegistryValue(HWND hwnd, HKEY hKey, LPCTSTR lpValueName) {
	DWORD dwType;
	DWORD dwSize = 0;

	// ѕолучение размера значени€
	if (RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, &dwSize) != ERROR_SUCCESS) {
		MessageBox(hwnd, L"Failed to get value size.", L"Error", MB_ICONERROR | MB_OK);
		return;
	}

	// ¬ыделение пам€ти дл€ значени€
	std::vector<BYTE> data(dwSize);

	// „тение значени€
	if (RegQueryValueEx(hKey, lpValueName, NULL, &dwType, &data[0], &dwSize) != ERROR_SUCCESS) {
		MessageBox(hwnd, L"Failed to read value.", L"Error", MB_ICONERROR | MB_OK);
		return;
	}

	// ѕреобразование данных в строку
	std::wstring valueString;
	if (dwType == REG_SZ || dwType == REG_EXPAND_SZ) {
		valueString = reinterpret_cast<wchar_t*>(&data[0]);
	} else if (dwType == REG_MULTI_SZ) {
		// REG_MULTI_SZ состоит из нескольких строк, разделенных нулевыми символами
		wchar_t* pStr = reinterpret_cast<wchar_t*>(&data[0]);
		while (*pStr) {
			valueString += pStr;
			valueString += L"\r\n";
			pStr += wcslen(pStr) + 1;
		}
	} else if (dwType == REG_DWORD) {
		DWORD value = *reinterpret_cast<DWORD*>(&data[0]);
		std::wstringstream ss;
		ss << L"0x" << std::uppercase << std::setfill(L'0') << std::setw(sizeof(DWORD) * 2) << std::hex << value;
		valueString = ss.str();
	} else {
		MessageBox(hwnd, L"Unsupported value type.", L"Error", MB_ICONERROR | MB_OK);
		return;
	}

	// ќтображение диалогового окна дл€ изменени€ значени€
	INT_PTR result = DialogBoxParam(NULL, MAKEINTRESOURCE(IDC_EDIT_VALUE), hwnd, EditValueDialogProc, reinterpret_cast<LPARAM>(&valueString));
	if (result == IDOK) {
		// ѕреобразование строки обратно в данные
		std::vector<BYTE> newData;
		if (dwType == REG_SZ || dwType == REG_EXPAND_SZ) {
			newData.assign(valueString.begin(), valueString.end());
			newData.push_back(0); // добавление нулевого символа
		} else if (dwType == REG_MULTI_SZ) {
			// –азделение строки на несколько строк, разделенных нулевыми символами
			std::wstringstream ss(valueString);
			std::wstring line;
			while (std::getline(ss, line)) {
				newData.insert(newData.end(), line.begin(), line.end());
				newData.push_back(0); // добавление нулевого символа
			}
			newData.push_back(0); // добавление дополнительного нулевого символа в конце
		} else if (dwType == REG_DWORD) {
			DWORD value = 0;
			std::wstringstream ss(valueString);
			ss >> std::hex >> value;
			newData.resize(sizeof(DWORD));
			*reinterpret_cast<DWORD*>(&newData[0]) = value;
		} else {
			MessageBox(hwnd, L"Unsupported value type.", L"Error", MB_ICONERROR | MB_OK);
			return;
		}

		// »зменение значени€ реестра
		if (RegSetValueEx(hKey, lpValueName, NULL, dwType, &newData[0], static_cast<DWORD>(newData.size())) != ERROR_SUCCESS) {
			MessageBox(hwnd, L"Failed to set value.", L"Error", MB_ICONERROR | MB_OK);
			return;
		}

		MessageBox(hwnd, L"Value modified successfully.", L"Success", MB_ICONINFORMATION | MB_OK);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_NOTIFY:
	{
		NMHDR* pnmhdr = (NMHDR*)lParam;
		if (pnmhdr->code == NM_DBLCLK) {
			// ѕолучение выбранного элемента
			HTREEITEM hSelectedItem = (HTREEITEM)SendMessage(hTreeView, TVM_GETNEXTITEM, TVGN_CARET, 0);
			if (hSelectedItem != NULL) {
				TVITEM tvItem;
				tvItem.mask = TVIF_PARAM;
				tvItem.hItem = hSelectedItem;
				SendMessage(hTreeView, TVM_GETITEM, 0, (LPARAM)&tvItem);

				HKEY hKey = (HKEY)tvItem.lParam;
				wchar_t szValueName[MAX_PATH];
				DWORD dwValueNameSize = sizeof(szValueName);
				DWORD dwType;
				DWORD dwDataSize = 0;

				// ѕолучение имени и типа значени€ выбранного ключа реестра
				if (RegEnumValue(hKey, 0, szValueName, &dwValueNameSize, NULL, &dwType, NULL, &dwDataSize) == ERROR_SUCCESS) {
					// ќткрытие диалогового окна дл€ изменени€ значени€
					EditRegistryValue(hwnd, hKey, szValueName);
				}
			}

			break;
		}
		break;
	}


	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

INT_PTR CALLBACK EditValueDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
	{
		// ѕолучение указател€ на строку значени€ из параметров диалогового окна
		LPWSTR lpValueString = reinterpret_cast<LPWSTR>(lParam);

		// ”становка текущего значени€ в поле редактировани€
		SetDlgItemText(hwndDlg, IDC_EDIT_VALUE, lpValueString);
		SetFocus(GetDlgItem(hwndDlg, IDC_EDIT_VALUE));

		return FALSE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		{
			// ѕолучение указател€ на строку значени€ из пользовательского данных окна
			LPWSTR lpValueString = reinterpret_cast<LPWSTR>(GetWindowLongPtr(hwndDlg, GWLP_USERDATA));

			// ѕолучение значени€ из пол€ редактировани€
			std::wstring valueString;
			int length = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_EDIT_VALUE)) + 1;
			GetDlgItemText(hwndDlg, IDC_EDIT_VALUE, &valueString[0], length);

			// ќбновление значени€ в пользовательском буфере
			lpValueString = &valueString[0]; // ???

			EndDialog(hwndDlg, IDOK);
			return TRUE;
		}
		case IDCANCEL:
			EndDialog(hwndDlg, IDCANCEL);
			return TRUE;
		}
		break;
	}

	return FALSE;
}