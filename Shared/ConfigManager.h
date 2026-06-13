#pragma once
#include <windows.h>
#include <string>
#include <shlobj.h> // For SHGetFolderPath
#include <shlwapi.h>

class ConfigManager {
    static std::wstring GetConfigPath() {
        wchar_t path[MAX_PATH];
        SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path);
        std::wstring fullPath = std::wstring(path) + L"\\GameBooster\\config.ini";
        return fullPath;
    }

    static void EnsureConfigDirExists() {
        std::wstring full = GetConfigPath();
        size_t pos = full.find_last_of(L"\\/");
        if (pos != std::wstring::npos) {
            std::wstring dir = full.substr(0, pos);
            // Create directory (and parents) if needed
            SHCreateDirectoryExW(NULL, dir.c_str(), NULL);
        }
    }

public:
    static void SaveSetting(std::wstring key, std::wstring value) {
        EnsureConfigDirExists();
        WritePrivateProfileStringW(L"Settings", key.c_str(), value.c_str(), GetConfigPath().c_str());
    }

    static std::wstring GetSetting(std::wstring key) {
        EnsureConfigDirExists();
        wchar_t buffer[256];
        GetPrivateProfileStringW(L"Settings", key.c_str(), L"", buffer, 256, GetConfigPath().c_str());
        return std::wstring(buffer);
    }
};
