#include <windows.h>
// Use standard library to manage process loops and settings
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Hidden background loop: no console window created
    while (true) {
        // TODO: Load settings, check processes, terminate if necessary
        Sleep(5000); // Check every 5 seconds
    }
    return 0;
}
