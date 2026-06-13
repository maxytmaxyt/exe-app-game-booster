; GameBooster Inno Setup Script
[Setup]
AppName=GameBooster
AppVersion=1.0
DefaultDirName={pf}\GameBooster
DefaultGroupName=GameBooster
OutputBaseFilename=GameBoosterInstaller
OutputDir=dist
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Files]
Source: "bin\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\GameBooster Configuration"; Filename: "{app}\ConfigUI.exe"
Name: "{commondesktop}\GameBooster"; Filename: "{app}\ConfigUI.exe"; Tasks: desktopicon


[Run]
; Ensure MonitorService is scheduled to run at system startup as SYSTEM
Filename: "{sys}\schtasks.exe"; Parameters: "/Create /SC ONSTART /TN \"GameBoosterMonitor\" /TR \"\"{app}\\MonitorService.exe\"\" /RL HIGHEST /F /RU SYSTEM"; Flags: runhidden

Filename: "{app}\ConfigUI.exe"; Description: "Launch GameBooster"; Flags: nowait postinstall skipifsilent

[UninstallRun]
Filename: "{sys}\schtasks.exe"; Parameters: "/Delete /TN \"GameBoosterMonitor\" /F"; Flags: runhidden

[UninstallDelete]
Type: filesandordirs; Name: "{app}"
