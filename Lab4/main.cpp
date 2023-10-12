#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <regex>

// Идентификатор элемента управления списка
#define IDC_PROCESS_LIST 1001
#define IDC_TERMINATE_BUTTON 1002
#define IDC_SUSPEND_BUTTON 1003
#define IDC_RESUME_BUTTON 1004

HWND hwndList = NULL;
HWND buttonTerminate = NULL;
HWND buttonSuspend = NULL;
HWND buttonResume = NULL;

DWORD selectedPId = -1;
std::regex processRegex("\((\d+)\)");

// Обработчик сообщений окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {


	case WM_CREATE:
	{

		buttonTerminate = CreateWindow(
			L"BUTTON",
			L"Terminate",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			10, 19, 100, 30, hwnd, (HMENU)IDC_TERMINATE_BUTTON, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL
		);

		buttonSuspend= CreateWindow(
			L"BUTTON",
			L"Suspend",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			120, 10, 100, 30,
			hwnd,
			(HMENU)IDC_SUSPEND_BUTTON,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL
		);

		buttonResume = CreateWindow(
			L"BUTTON",
			L"Button 3",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			230, 10, 100, 30,
			hwnd,
			(HMENU)IDC_RESUME_BUTTON,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL
		);


		hwndList = CreateWindowEx(
			0,
			L"LISTBOX",
			NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY | LBS_HASSTRINGS,
			10, 50, 780, 500, hwnd,
			NULL,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL
		);


		if (hwndList == NULL) {
			MessageBoxA(NULL, "Failed to create list box", "Error", MB_OK | MB_ICONERROR);
			return 0;
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

	case WM_COMMAND:
	{
		if (HIWORD(wParam) == LBN_SELCHANGE) {
			int selectedIndex = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			if (selectedIndex != LB_ERR) {
				int textLen = SendMessage(hwndList, LB_GETTEXTLEN, (WPARAM)selectedIndex, 0);
				wchar_t* result = new wchar_t[textLen + 1];
				SendMessage(hwndList, LB_GETTEXT, (WPARAM)selectedIndex, reinterpret_cast<LPARAM>(result));
				std::wstring res(result);
				std::string res1(res.begin(), res.end());
				std::cmatch m;
				if (std::regex_search(res1.c_str(), m, processRegex)) {
					res1 = "1";
				}
				auto words_begin = std::sregex_iterator(res1.begin(), res1.end(), processRegex);
				auto words_end = std::sregex_iterator();
				selectedPId = atoi((*(++words_begin)).str().c_str());
			}
		}
		return 0;
	}

	case WM_SIZE:
	{
		int buttonWidth = 100;
		int buttonHeight = 30;
		int buttonMargin = 10;
		int listBoxMarginTop = 50;
		int listBoxMarginBottom = 10;

		int windowWidth = LOWORD(lParam);
		int windowHeight = HIWORD(lParam);

		MoveWindow(buttonTerminate, buttonMargin, buttonMargin, buttonWidth, buttonHeight, true);
		MoveWindow(buttonSuspend, buttonMargin + buttonWidth + buttonMargin, buttonMargin, buttonWidth, buttonHeight, true);
		MoveWindow(buttonResume, buttonMargin + buttonWidth + buttonMargin + buttonWidth + buttonMargin, buttonMargin, buttonWidth, buttonHeight, true);
		MoveWindow(hwndList, buttonMargin, listBoxMarginTop, windowWidth - 2 * buttonMargin, windowHeight - listBoxMarginTop - listBoxMarginBottom, true);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
// Функция для вывода сообщения об ошибке и завершения приложения
VOID showErrorAndExit(const char* errorMessage) {
	MessageBoxA(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
	exit(EXIT_FAILURE);
}

// Функция для приостановки процесса по его идентификатору
VOID suspendProcess(DWORD processId) {
	HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processId);
	if (hProcess == NULL) {
		showErrorAndExit("Failed to open process");
	}

	if (SuspendThread(hProcess) == (DWORD)-1) {
		showErrorAndExit("Failed to suspend process");
	}

	CloseHandle(hProcess);
}

// Функция для возобновления процесса по его идентификатору
VOID resumeProcess(DWORD processId) {
	HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processId);
	if (hProcess == NULL) {
		showErrorAndExit("Failed to open process");
	}

	if (ResumeThread(hProcess) == (DWORD)-1) {
		showErrorAndExit("Failed to resume process");
	}

	CloseHandle(hProcess);
}

// Функция для завершения процесса по его идентификатору
VOID terminateProcess(DWORD processId) {
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
	if (hProcess == NULL) {
		showErrorAndExit("Failed to open process");
	}

	if (!TerminateProcess(hProcess, 0)) {
		showErrorAndExit("Failed to terminate process");
	}

	CloseHandle(hProcess);
}



// Функция для получения списка процессов
VOID GetProcessList(HWND hwndList) {
	// Открываем снимок состояния процессов
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		MessageBoxA(NULL, "Failed to create process snapshot", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Заполняем структуру с информацией о процессе
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Получаем первый процесс в снимке состояния
	if (!Process32First(hSnapshot, &pe32)) {
		CloseHandle(hSnapshot);
		MessageBoxA(NULL, "Failed to retrieve process information", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Очищаем список
	SendMessage(hwndList, LB_RESETCONTENT, 0, 0);

	// Добавляем идентификаторы и имена процессов в список
	do {
		std::wstring processName = std::wstring(pe32.szExeFile) + L" (" + std::to_wstring(pe32.th32ProcessID) + L")";

		INT pId = pe32.th32ProcessID;

		SendMessageW(hwndList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(processName.c_str()));
	} while (Process32Next(hSnapshot, &pe32));

	// Закрываем дескриптор снимка состояния
	CloseHandle(hSnapshot);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Регистрируем класс окна
	const wchar_t CLASS_NAME[] = L"ProcessListWindowClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Создаем окно
	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Process List",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL) {
		MessageBoxA(NULL, "Failed to create window", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	// Получаем список процессов и добавляем его в список
	GetProcessList(hwndList);

	// Отображаем окно
	ShowWindow(hwnd, nCmdShow);

	// Запускаем цикл обработки сообщений
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}