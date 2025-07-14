#pragma once

#include <cstdint>
#define DLL_EXPORT extern "C" __declspec(dllexport)
#include <crow.h>

bool __stdcall DllMain(void *, std::uint32_t reason, void *);

void Run();
