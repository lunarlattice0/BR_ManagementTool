#pragma once

#include <cstdint>
#include <memory>
#define DLL_EXPORT extern "C" __declspec(dllexport)
#include <crow.h>

// Offsets
constexpr uintptr_t GObjects_Offset = 0x043d2600;

constexpr uintptr_t GWorld_Offset = 0x04515F18;
constexpr uintptr_t GWorld_PlayerControllerList_Offset = GWorld_Offset + 0x1c0;

constexpr uintptr_t ABrickGameMode_Get_Offset = 0x0ce0460;
constexpr uintptr_t ABrickGameMode_EndMatch_Offset = 0x0cde190;
constexpr uintptr_t ABrickGameMode_RestartGame_Offset = 0x0cffc20;
constexpr uintptr_t ABrickGameMode_RestartAllPlayers_Offset = 0x0cffb60;
constexpr uintptr_t ABrickGameMode_EndRound_Offset = 0x0cde330;

constexpr uintptr_t ABrickPlayerController_AdminSay_Offset = 0x140d139a0;
constexpr uintptr_t ABrickPlayerController_KillCharacter_Offset = 0x140d2da90;
constexpr uintptr_t ABrickPlayerController_ExplodeVehicle_Offset = 0x140d1e220;
constexpr uintptr_t ABrickPlayerController_Say_Offset = 0x140d3c460;
constexpr uintptr_t ABrickPlayerController_ScrapAllVehicles_Offset = 0x140d3c470;

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

// WIP
class APlayerController : private BrickRigsInternalClass {
    public:
        APlayerController(void* address);
        void* GetCurrentAdr();
};

class GWorld final : public BrickRigsInternalClass {

    public:
        GWorld();
        void* GetCurrentAdr();
};

class ABrickGameMode final : public BrickRigsInternalClass {
    public:
        ABrickGameMode(void* gworld);
        void* GetCurrentAdr();
};

// ABrickPlayerController not implemented; please use ABrickGameMode to get existing players.

inline bool InternalClassExists(std::shared_ptr<BrickRigsInternalClass> bric) {
    if (bric->GetCurrentAdr() != 0) {
        return true;
    } else {
        return false;
    }
}
