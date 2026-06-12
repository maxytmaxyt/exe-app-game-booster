#pragma once
#include <string>

// Header-only helper to manage the shared configuration file
class ConfigManager {
public:
    static void SaveSetting(std::string key, std::string value) {
        // Write to %APPDATA%\GameBooster\config.ini
    }
    static std::string GetSetting(std::string key) {
        // Read from %APPDATA%\GameBooster\config.ini
        return "";
    }
};
