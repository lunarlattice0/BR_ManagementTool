#pragma once

#include <cstdint>
#include <memory>
#define DLL_EXPORT extern "C" __declspec(dllexport)
#include <crow.h>

// Offsets
constexpr uintptr_t GWorldOffset = 0x04515F18;
constexpr uintptr_t ABrickGameMode_Get_Offset = 0x0ce0460;
constexpr uintptr_t ABrickGameMode_EndMatch_Offset = 0x0cde190;
constexpr uintptr_t ABrickGameMode_RestartGame_Offset = 0x0cffc20;
constexpr uintptr_t ABrickGameMode_RestartAllPlayers_Offset = 0x0cffb60;

// DLL Entry
bool __stdcall DllMain(void *, std::uint32_t reason, void *);

// Main loop
void Run();

// Screenshots
bool save_png_memory(HBITMAP hbitmap, std::vector<BYTE>&data);
std::vector<BYTE> GetScreenshot();

class BrickRigsInternalClass {
    public:
        virtual void * GetCurrentAdr() = 0;
        // Caution, never store a pointer to internal objects.
        // UE4 may dtor the object, and we would be doing a use-after-free.
    protected:
        void * gworldptr;
};

// GWorld
class GWorld final : public BrickRigsInternalClass {
    public:
        GWorld();
        void* GetCurrentAdr();
    private:
        void * FuncAdr;

};

// ABrickGameMode
class ABrickGameMode final : public BrickRigsInternalClass {
    public:
        ABrickGameMode(void* gworld);
        void* GetCurrentAdr();
};

inline bool InternalClassExists(std::shared_ptr<BrickRigsInternalClass> bric) {
    if (bric->GetCurrentAdr() != 0) {
        return true;
    } else {
        return false;
    }
}
