#pragma once
#include <windows.h>
#include <vector>

class CpuMonitor {
public:
    static double GetProcessCpuUsage(HANDLE hProcess) {
        FILETIME creationTime, exitTime, kernelTime, userTime;
        if (!GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {
            return 0.0;
        }

        // Combine kernel and user time
        ULARGE_INTEGER kernel, user;
        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;

        return (double)(kernel.QuadPart + user.QuadPart);
    }
};
