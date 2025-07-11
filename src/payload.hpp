#pragma once
#include <cstdint>
#define DLL_EXPORT extern "C" __declspec(dllexport)
#include <cstddef>
#include <windows.h>

//BOOL WINAPI DllMain(HANDLE hDll, DWORD dwReason, LPVOID LpReserved);
bool __stdcall DllMain(void *, std::uint32_t reason, void *);
