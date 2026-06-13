#include <windows.h>
#include <shellapi.h>
#include <string>
#include "../../Shared/ConfigManager.h"

// Simple dialog-like UI
static const wchar_t CLASS_NAME[] = L"GameBoosterUIClass";
enum { IDC_EDIT = 101, IDC_SAVE = 102, IDC_INSTALL = 103 };

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        CreateWindowExW(0, L"STATIC", L"CPU Threshold (percent):", WS_CHILD | WS_VISIBLE, 10, 10, 180, 20, hwnd, NULL, NULL, NULL);
        HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_LEFT, 10, 35, 180, 24, hwnd, (HMENU)IDC_EDIT, NULL, NULL);
        std::wstring current = ConfigManager::GetSetting(L"CpuThreshold");
        if (!current.empty()) SetWindowTextW(hEdit, current.c_str());
        CreateWindowW(L"BUTTON", L"Save", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 70, 80, 26, hwnd, (HMENU)IDC_SAVE, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Install", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 110, 70, 80, 26, hwnd, (HMENU)IDC_INSTALL, NULL, NULL);
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == IDC_SAVE) {
            wchar_t buffer[64] = {0};
            GetWindowTextW(GetDlgItem(hwnd, IDC_EDIT), buffer, 64);
            std::wstring val(buffer);
            if (val.empty()) {
                MessageBoxW(hwnd, L"Please enter a value.", L"GameBooster", MB_OK | MB_ICONWARNING);
            } else {
                ConfigManager::SaveSetting(L"CpuThreshold", val);
                MessageBoxW(hwnd, L"Saved.", L"GameBooster", MB_OK | MB_ICONINFORMATION);
            }
        } else if (id == IDC_INSTALL) {
            // Try to run the local install.bat (created by build)
            HINSTANCE r = ShellExecuteW(NULL, L"open", L"install.bat", NULL, NULL, SW_SHOWNORMAL);
            if ((INT_PTR)r <= 32) {
                MessageBoxW(hwnd, L"Could not launch installer. Run install.bat manually.", L"GameBooster", MB_OK | MB_ICONERROR);
            }
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static std::wstring GetExeDir() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring s(exePath);
    size_t pos = s.find_last_of(L"\\/");
    if (pos != std::wstring::npos) return s.substr(0, pos);
    return s;
}

static bool FileExists(const std::wstring &path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

static bool IsInstalled() {
    wchar_t buf[4096];
    DWORD len = GetEnvironmentVariableW(L"ProgramFiles", buf, _countof(buf));
    if (len == 0 || len >= _countof(buf)) return false;
    std::wstring prog = std::wstring(buf) + L"\\GameBooster\\MonitorService.exe";
    return FileExists(prog);
}

static void LaunchInstaller() {
    std::wstring exeDir = GetExeDir();
    std::wstring try1 = exeDir + L"\\install.bat";
    std::wstring try2;
    size_t pos = exeDir.find_last_of(L"\\/");
    if (pos != std::wstring::npos) try2 = exeDir.substr(0, pos) + L"\\install.bat";

    LPCWSTR target = NULL;
    if (FileExists(try1)) target = try1.c_str();
    else if (!try2.empty() && FileExists(try2)) target = try2.c_str();

    HINSTANCE r;
    if (target) {
        r = ShellExecuteW(NULL, L"runas", target, NULL, NULL, SW_SHOWNORMAL);
    } else {
        r = ShellExecuteW(NULL, L"runas", L"install.bat", NULL, NULL, SW_SHOWNORMAL);
    }
    if ((INT_PTR)r <= 32) {
        MessageBoxW(NULL, L"Installer konnte nicht gestartet werden. Bitte install.bat manuell ausführen.", L"Installation fehlgeschlagen", MB_OK | MB_ICONERROR);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // If not installed, launch installer and exit
    if (!IsInstalled()) {
        LaunchInstaller();
        return 0;
    }
    WNDCLASSW wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"GameBooster - Configuration", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 220, 150, NULL, NULL, hInstance, NULL);
    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
