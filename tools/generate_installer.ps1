param(
    [string]$OutDir = (Join-Path $PWD "dist")
)

$ErrorActionPreference = 'Stop'

if (-not (Test-Path $OutDir)) {
    New-Item -ItemType Directory -Path $OutDir | Out-Null
}

$files = @(
    (Join-Path $PWD 'bin\MonitorService.exe'),
    (Join-Path $PWD 'bin\ConfigUI.exe'),
    (Join-Path $PWD 'install.bat')
)

foreach ($f in $files) {
    if (-not (Test-Path $f)) {
        Write-Error "Datei nicht gefunden: $f"
        exit 1
    }
}

$cppPath = Join-Path $OutDir 'installer_builder.cpp'

$header = @'
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <iostream>

static std::vector<unsigned char> Base64Decode(const std::string& in) {
    static const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) {
        T[static_cast<unsigned char>(chars[i])] = i;
    }

    std::vector<unsigned char> out;
    int val = 0;
    int valb = -8;

    for (unsigned char c : in) {
        if (c == '=') break;
        if (T[c] == -1) continue;

        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(static_cast<unsigned char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return out;
}

static bool WriteBytes(const std::wstring& path, const std::vector<unsigned char>& data) {
    HANDLE h = CreateFileW(
        path.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (h == INVALID_HANDLE_VALUE) return false;

    DWORD written = 0;
    BOOL ok = TRUE;

    if (!data.empty()) {
        ok = WriteFile(h, data.data(), static_cast<DWORD>(data.size()), &written, NULL);
        if (!ok || written != data.size()) {
            CloseHandle(h);
            return false;
        }
    }

    CloseHandle(h);
    return true;
}

static bool IsAdmin() {
    BOOL f = FALSE;
    SID_IDENTIFIER_AUTHORITY nt = SECURITY_NT_AUTHORITY;
    PSID adminGroup = NULL;

    if (AllocateAndInitializeSid(
            &nt,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0,0,0,0,0,0,
            &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &f);
        FreeSid(adminGroup);
    }

    return f == TRUE;
}

int wmain() {
    if (!IsAdmin()) {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        ShellExecuteW(NULL, L"runas", path, NULL, NULL, SW_SHOWNORMAL);
        return 0;
    }

    wchar_t pf[MAX_PATH];
    ExpandEnvironmentStringsW(L"%ProgramFiles%\\GameBooster", pf, MAX_PATH);
    CreateDirectoryW(pf, NULL);

    std::wstring dest = pf;
'@

function Write-FileWithRetries {
    param($Path, $Value, $Encoding = 'utf8', $Retries = 5, $DelayMs = 250)

    for ($i = 1; $i -le $Retries; $i++) {
        try {
            Set-Content -Path $Path -Value $Value -Encoding $Encoding -ErrorAction Stop
            return
        }
        catch [System.IO.IOException] {
            if ($i -eq $Retries) { throw }
            Start-Sleep -Milliseconds $DelayMs
        }
    }
}

$index = 0
$content = $header
foreach ($f in $files) {
    $bytes = [IO.File]::ReadAllBytes($f)
    $b64   = [Convert]::ToBase64String($bytes)
    $name  = [IO.Path]::GetFileName($f)

    $content += "static const char DATA$index[] = `"$b64`";`n"
    $content += "static const wchar_t NAME$index[] = L`"$name`";`n"
    $index++
}

$footer = @"
    struct E { const wchar_t* n; const char* b; };
    E files[] = {
"@

for ($i = 0; $i -lt $index; $i++) {
    $footer += "`n        { NAME$i, DATA$i },"
}

$footer += @"
    };

    for (auto& f : files) {
        auto data = Base64Decode(f.b);
        if (!WriteBytes(dest + L"\\" + f.n, data)) {
            std::wcerr << L"Fehler beim Schreiben: " << f.n << std::endl;
            return 1;
        }
    }

    // Task/Shortcut Logik hier einfügen...
    return 0;
}
"@

$content += $footer

Write-FileWithRetries -Path $cppPath -Value $content -Encoding utf8

Write-Host "Kompiliere Installer..."
& g++ $cppPath -o (Join-Path $OutDir 'GameBoosterInstaller.exe') -municode -static -s -O2

if ($LASTEXITCODE -ne 0) {
    Write-Error "Kompilierung fehlgeschlagen."
    exit $LASTEXITCODE
}

Write-Host "Fertig: $(Join-Path $OutDir 'GameBoosterInstaller.exe')"