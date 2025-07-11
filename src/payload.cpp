#include "payload.hpp"
#include <consoleapi.h>
#include <windows.h>
#include <winnt.h>
#include <winuser.h>

bool __stdcall DllMain(void *, std::uint32_t reason, void *) {
    if (reason == DLL_PROCESS_ATTACH) {
        AllocConsole();
    }
    return true;
}

/*
BOOL WINAPI DllMain(HANDLE hDll, DWORD dwReason, LPVOID LpReserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            MessageBox(NULL, "TEST", "TEST", MB_OK);
        break;
    }
    return TRUE;
}
*/
