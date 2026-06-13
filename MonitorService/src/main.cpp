#include <windows.h>
#include <vector>
#include <string>
#include "../../Shared/ConfigManager.h"
#include "CpuMonitor.h"
#include <tlhelp32.h> // REQUIRED for process snapshot functions

void MonitorLoop(double threshold) {
    // 1. Snapshot all processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (hProcess) {
                // Measure initial time
                double time1 = CpuMonitor::GetProcessCpuUsage(hProcess);
                Sleep(100); // Small interval
                double time2 = CpuMonitor::GetProcessCpuUsage(hProcess);

                // Calculate usage (simplified logic)
                double usage = (time2 - time1) / 10000.0; // Conversion to percentage

                if (usage > threshold) {
                    TerminateProcess(hProcess, 1);
                }
                CloseHandle(hProcess);
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Create/open a global stop event that can be signaled by the UI.
    HANDLE hStopEvent = CreateEventW(NULL, TRUE, FALSE, L"Global\\GameBooster_StopEvent");

    // Read threshold from ProgramFiles config if present, otherwise default to 80
    wchar_t prog[MAX_PATH];
    ExpandEnvironmentStringsW(L"%ProgramFiles%\\GameBooster\\config.ini", prog, MAX_PATH);
    wchar_t buf[64] = {0};
    double threshold = 80.0;
    if (GetPrivateProfileStringW(L"Settings", L"CpuThreshold", L"", buf, _countof(buf), prog) > 0) {
        try {
            threshold = std::stod(std::wstring(buf));
        } catch (...) {
            threshold = 80.0;
        }
    }

    while (true) {
        // Exit quickly if stop event is signaled
        if (hStopEvent && WaitForSingleObject(hStopEvent, 0) == WAIT_OBJECT_0) {
            break;
        }

        MonitorLoop(threshold);
        Sleep(5000); // Main cycle
    }

    if (hStopEvent) CloseHandle(hStopEvent);
    return 0;
}
