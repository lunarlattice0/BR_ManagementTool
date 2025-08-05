#pragma once

#include <cstdint>
#define DLL_EXPORT extern "C" __declspec(dllexport)
#include <crow.h>

constexpr uintptr_t GWorldOffset = 0x04515F18;
constexpr uintptr_t ABrickGameMode_Get_Offset = 0x0ce0460;
class GWorld final {
    public:
        GWorld();
        void* GetCurrentAdr();
    private:
        void* GWorldAdr;

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
        ABrickGameMode(void* gworld);
    private:
        void *ABrickGameModeAdr;
};
