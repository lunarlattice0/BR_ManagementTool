#include "brlmt.hpp"
#include <cstddef>
#include <fileapi.h>
#include <fstream>
#include <iostream>
#include <libloaderapi.h>
#include <memoryapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <stdexcept>
#include <sstream>
#include <winnt.h>

void __thiscall Game::InjectDLL() {
    char dllPath[MAX_PATH]; //= "Z:\\home\\gaming\\libbrlmt_payload.dll";
    GetFullPathName(".\\libbrlmt_payload.dll", MAX_PATH, dllPath, NULL);
    size_t sizedll = sizeof(dllPath);

    this->remoteBuffer = VirtualAllocEx(this->hProcess, NULL,sizedll, (MEM_COMMIT | MEM_RESERVE), PAGE_READWRITE);
    if (this->remoteBuffer == NULL) {
        throw std::runtime_error("Couldn't allocate buffer.");
    }
    if (WriteProcessMemory(this->hProcess, this->remoteBuffer, (LPVOID)dllPath, sizedll, NULL) == 0) {
        throw std::runtime_error("Couldn't write dll.");
    }
    PTHREAD_START_ROUTINE startAdr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32"), "LoadLibraryA");
    if (startAdr == NULL) {
        throw std::runtime_error("Couldn't get start address.");
    }
    this->remoteThread = CreateRemoteThread(this->hProcess, NULL, 0, startAdr, this->remoteBuffer, 0, NULL);
    if (hProcess == NULL) {
        throw std::runtime_error("Couldn't start remote thread.");
    }

    std::cout << "--DIAGNOSTICS--" << std::endl;
    std::cout << "Buffer adr: " << remoteBuffer << std::endl;
    std::cout << "startAdr: " << startAdr << std::endl;
    std::cout << "remoteThread: " << remoteThread << std::endl;

    return;
}

Game::Game() {
    // shamelessly plagiarised from https://stackoverflow.com/questions/865152/how-can-i-get-a-process-handle-by-its-name-in-c
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    this->snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == TRUE) { [[likely]]
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (stricmp(entry.szExeFile, "BrickRigsSteam-Win64-Shipping.exe") == 0) {
                this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
            }
        }
    }

    if (this->hProcess == NULL) {
        throw std::runtime_error("No Brick Rigs process found.");
    }

    InjectDLL();
}

Game::~Game() {
    CloseHandle(this->hProcess);
    CloseHandle(this->snapshot);
}
