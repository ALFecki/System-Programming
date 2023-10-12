#include <windows.h>
#include <tlhelp32.h>
#include <string>

// ������������� �������� ���������� ������
#define IDC_PROCESS_LIST 1001


HWND hwndList = NULL;

// ���������� ��������� ����
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {


    case WM_CREATE:
    {
        // ������� ������ ��� ������ ���������
        hwndList = CreateWindowEx(
            0,
            L"LISTBOX",
            NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_HASSTRINGS,
            10, 10, 780, 540,
            hwnd,
            NULL,
            (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
            NULL
        );

        if (hwndList == NULL) {
            MessageBoxA(NULL, "Failed to create list box", "Error", MB_OK | MB_ICONERROR);
            return 0;
        }
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
        MoveWindow(hwndList, 0, 0, LOWORD(lParam), HIWORD(lParam), false);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
// ������� ��� ������ ��������� �� ������ � ���������� ����������
VOID showErrorAndExit(const char* errorMessage) {
    MessageBoxA(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
    exit(EXIT_FAILURE);
}

// ������� ��� ������������ �������� �� ��� ��������������
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

// ������� ��� ������������� �������� �� ��� ��������������
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

// ������� ��� ���������� �������� �� ��� ��������������
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



// ������� ��� ��������� ������ ���������
VOID GetProcessList(HWND hwndList) {
    // ��������� ������ ��������� ���������
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        MessageBoxA(NULL, "Failed to create process snapshot", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // ��������� ��������� � ����������� � ��������
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // �������� ������ ������� � ������ ���������
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        MessageBoxA(NULL, "Failed to retrieve process information", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // ������� ������
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0);

    // ��������� �������������� � ����� ��������� � ������
    do {
        LPWSTR processName = pe32.szExeFile;
        LPWSTR processId = new WCHAR[10];
        swprintf(processId, 10, L"%lu", pe32.th32ProcessID);
        size_t str1Length = wcslen(processName);
        size_t str2Length = wcslen(processId);

        LPWSTR result = new WCHAR[str1Length + str2Length + 1];
        wcscpy_s(result, str1Length + 1, processName);
        wcscat_s(result, str1Length + str2Length + 1, processId);

        //DWORD pId = pe32.th32ProcessID;
        SendMessageW(hwndList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(result));
    } while (Process32Next(hSnapshot, &pe32));

    // ��������� ���������� ������ ���������
    CloseHandle(hSnapshot);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // ������������ ����� ����
    const wchar_t CLASS_NAME[] = L"ProcessListWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // ������� ����
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

    // �������� ������ ��������� � ��������� ��� � ������
    GetProcessList(hwndList);

    // ���������� ����
    ShowWindow(hwnd, nCmdShow);

    // ��������� ���� ��������� ���������
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}