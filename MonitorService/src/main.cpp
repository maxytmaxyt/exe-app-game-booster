#include <windows.h>
#include "../../Shared/ConfigManager.h"

// Entry point for a background application without console
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    bool running = true;
    
    while (running) {
        // Read threshold from config
        std::wstring threshold = ConfigManager::GetSetting(L"CpuThreshold");
        
        // TODO: Implement Logic:
        // 1. Snapshot processes
        // 2. Measure CPU usage per process
        // 3. Terminate if > threshold
        
        Sleep(5000); // Check every 5 seconds to remain lightweight
    }
    return 0;
}
