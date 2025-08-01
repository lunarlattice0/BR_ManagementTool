#pragma once

#include <cstdint>
#define DLL_EXPORT extern "C" __declspec(dllexport)
#include <crow.h>

class UWorld final {
    public:
        UWorld();
    private:
        uintptr_t UWorldAdr;

};

// DLL Entry
bool __stdcall DllMain(void *, std::uint32_t reason, void *);

// Main loop
void Run();

// Supporting functions

// Screenshots
bool save_png_memory(HBITMAP hbitmap, std::vector<BYTE>&data);
std::vector<BYTE> GetScreenshot();

// ABrickGameMode
class ABrickGameMode final {
    public:
        ABrickGameMode();
    private:
        uintptr_t ABrickGameModeAdr();
};
