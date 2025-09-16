#pragma once

#include <cstdint>
#include <memory>
#define DLL_EXPORT extern "C" __declspec(dllexport)
#include <crow.h>

// Offsets
#include "offsets.hpp"

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

class ABrickGameSession final : public BrickRigsInternalClass {
    public:
        ABrickGameSession(void * gworld);
        void * GetCurrentAdr();
};

inline bool InternalClassExists(std::shared_ptr<BrickRigsInternalClass> bric) {
    if (bric->GetCurrentAdr() != 0) {
        return true;
    } else {
        return false;
    }
};
