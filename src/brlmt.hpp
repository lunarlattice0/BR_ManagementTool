// This file contains general initial injection / server management functions.

#pragma once
#include <minwindef.h>
#include <processthreadsapi.h>
#include <windows.h>
#include <tlhelp32.h>
#include <winnt.h>

class Game {
    private:
        HANDLE snapshot = NULL;
        HANDLE hProcess = NULL;
        HANDLE remoteThread = NULL;
        HANDLE remoteBuffer = NULL;
    public:
        Game(); // Hook the game
        ~Game();
        void InjectDLL();
};
