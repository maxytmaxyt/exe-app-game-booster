#include <windows.h>
#include <vector>
#include "../../Shared/ConfigManager.h"
#include "CpuMonitor.h"
#include <tlhelp32.h> // REQUIRED for process snapshot functions

void MonitorLoop() {
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

                if (usage > 80.0) { // Threshold from config
                    TerminateProcess(hProcess, 1);
                }
                CloseHandle(hProcess);
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    while (true) {
        MonitorLoop();
        Sleep(5000); // Main cycle
    }
    return 0;
}
