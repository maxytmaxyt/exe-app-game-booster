#pragma once
#include <windows.h>
#include <string>
#include <shlobj.h> // For SHGetFolderPath

class ConfigManager {
    static std::wstring GetConfigPath() {
        wchar_t path[MAX_PATH];
        SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path);
        std::wstring fullPath = std::wstring(path) + L"\\GameBooster\\config.ini";
        return fullPath;
    }

public:
    static void SaveSetting(std::wstring key, std::wstring value) {
        WritePrivateProfileStringW(L"Settings", key.c_str(), value.c_str(), GetConfigPath().c_str());
    }

    static std::wstring GetSetting(std::wstring key) {
        wchar_t buffer[256];
        GetPrivateProfileStringW(L"Settings", key.c_str(), L"", buffer, 256, GetConfigPath().c_str());
        return std::wstring(buffer);
    }
};
