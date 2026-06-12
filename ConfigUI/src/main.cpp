#include <windows.h>
#include "../../Shared/ConfigManager.h"

// Simple UI entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Example: Update the threshold setting
    ConfigManager::SaveSetting(L"CpuThreshold", L"80");

    MessageBox(NULL, L"Settings saved to config.ini", L"GameBooster UI", MB_OK | MB_ICONINFORMATION);
    return 0;
}
