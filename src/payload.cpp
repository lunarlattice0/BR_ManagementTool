#include "payload.hpp"
#include "crow/app.h"
#include "crow/common.h"
#include "crow/http_response.h"
#include "crow/json.h"
#include <consoleapi.h>
#include <cstddef>
#include <cstdint>
#include <libloaderapi.h>
#include <memory>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <windows.h>
#include <winscard.h>
#include <crow.h>
#include <gdiplus.h>
#include <gdiplus/gdiplusheaders.h>

#include <ctime>

#ifndef BINDADDR
#define BINDADDR "0.0.0.0"
#endif

#ifndef BINDPORT
#define BINDPORT 8080
#endif

// Global variables
// Please note, you MUST refresh these globals. There is no guarantee that they will be valid without a refresh.
std::shared_ptr<GWorld> activeGWorld;
std::shared_ptr<ABrickGameMode> activeABrickGameMode;

// Refresh globalvars
inline void refreshGlobals() {
    activeGWorld = std::make_shared<GWorld>();
    activeABrickGameMode = std::make_shared<ABrickGameMode>(activeGWorld->GetCurrentAdr());
};

// Supporting function for converting HBITMAP to png
// Copied from https://stackoverflow.com/a/51388079, by Barmak Shemirani
bool save_png_memory(HBITMAP hbitmap, std::vector<BYTE> &data) {
    Gdiplus::Bitmap bmp(hbitmap, nullptr);

    //write to IStream
    IStream* istream = nullptr;
    if (CreateStreamOnHGlobal(NULL, TRUE, &istream) != 0)
        return false;

    CLSID clsid_png;
    if (CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &clsid_png)!=0)
        return false;
    Gdiplus::Status status = bmp.Save(istream, &clsid_png, NULL);
    if (status != Gdiplus::Status::Ok)
        return false;

    //get memory handle associated with istream
    HGLOBAL hg = NULL;
    if (GetHGlobalFromStream(istream, &hg) != S_OK)
        return 0;

    //copy IStream to buffer
    int bufsize = GlobalSize(hg);
    data.resize(bufsize);

    //lock & unlock memory
    LPVOID pimage = GlobalLock(hg);
    if (!pimage)
        return false;
    memcpy(&data[0], pimage, bufsize);
    GlobalUnlock(hg);

    istream->Release();
    return true;
}

// Get screenshot of display
std::vector<BYTE> GetScreenshot() {
    CoInitialize(NULL);
    ULONG_PTR token;
    Gdiplus::GdiplusStartupInput tmp;
    Gdiplus::GdiplusStartup(&token, &tmp, NULL);

    // yoinked from https://stackoverflow.com/a/3291411, by Woody
    // Acquire screenshot as HBITMAP
    HDC hScreenDC = GetDC(nullptr);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    int width = GetDeviceCaps(hScreenDC,HORZRES);
    int height = GetDeviceCaps(hScreenDC,VERTRES);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC,width,height);
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC,hBitmap));
    BitBlt(hMemoryDC,0,0,width,height,hScreenDC,0,0,SRCCOPY);
    hBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC,hOldBitmap));
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    std::vector<BYTE> data;
    if (!save_png_memory(hBitmap, data)) {
        data.clear();
        // Check if length 0 in caller.
    }
    Gdiplus::GdiplusShutdown(token);
    CoUninitialize();
    return data;
}

// Class Initializers

GWorld::GWorld() {
    this->FuncAdr = *reinterpret_cast<void**>((uintptr_t)GetModuleHandle(NULL) + GWorldOffset);
    this->gworldptr = FuncAdr;
}

void* GWorld::GetCurrentAdr() {
    return this->FuncAdr;
}

ABrickGameMode::ABrickGameMode(void* gworld) {
    this->gworldptr = gworld;
}

void * ABrickGameMode::GetCurrentAdr() {
    auto getFunctionAdr = (uintptr_t)GetModuleHandle(NULL) + ABrickGameMode_Get_Offset;
    using getfn = void* (__cdecl *) (void* UObject);
    getfn get = reinterpret_cast<getfn>(getFunctionAdr);
    return reinterpret_cast<void*>(get(this->gworldptr));
}

// Main function
void Run() {
    // Populate GWorld, since it will always be available.
    activeGWorld = std::make_shared<GWorld>();
    // Load a dummy ABrickGameMode
    activeABrickGameMode = std::make_shared<ABrickGameMode>(activeGWorld->GetCurrentAdr());

    crow::SimpleApp app;

    // ABrickGameMode Funcs
    CROW_ROUTE(app, "/ABrickGameMode")([&](){
        crow::json::wvalue response;
        response["/get"] = "Get active ABrickGameMode. In essence, check if in game.";
        response["/end/match"] = "End the current match.";
        response["/end/round"] = "End the current round.";
        response["/restart/game"] = "Restart the current game.";
        response["/restart/allPlayers"] = "Restart all players (including dead players) to spawn.";
        response["/kill/<player>"] = "Kill a player. Not implemented.";
        response["/kill/vehicle/<player>"] = "Remove a player's vehicle. Not implemented.";
        response["/kill/allVehicles"] = "Remove all vehicles. Not implemented.";
        return response;
    });

    // In essence, check if in game.
    CROW_ROUTE(app, "/ABrickGameMode/get")([&](){
        refreshGlobals();

        if (InternalClassExists(activeABrickGameMode)) {
            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    // End a match
    CROW_ROUTE(app, "/ABrickGameMode/end/match")([&](){
        refreshGlobals();

        if (InternalClassExists(activeABrickGameMode)) {
            auto endMatchFunctionAdr = (uintptr_t) GetModuleHandleA(NULL) + ABrickGameMode_EndMatch_Offset;
            using fn = void (__thiscall *)(void * ABrickGameMode);
            fn func = reinterpret_cast<fn>(endMatchFunctionAdr);
            func(activeABrickGameMode->GetCurrentAdr());

            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/end/round")([&](){
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode)) {
            auto endRoundFunctionAdr = (uintptr_t) GetModuleHandleA(NULL) + ABrickGameMode_EndRound_Offset;
            using fn = void(__thiscall *)(void * ABrickGameMode, void* matchWinner, bool lastRound);
            fn func = reinterpret_cast<fn>(endRoundFunctionAdr);
            // Use a fake team struct
            typedef struct {
                unsigned char TeamID;
            }__attribute__((packed, aligned(1))) stubbedTeam;
            stubbedTeam * fakeTeam = new stubbedTeam{};
            fakeTeam->TeamID = 0;
            func(activeABrickGameMode->GetCurrentAdr(), &fakeTeam, false);
            delete(fakeTeam);
            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/restart/game")([&]() {
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode)) {
            auto restartGameFunctionAdr = (uintptr_t) GetModuleHandleA(NULL) + ABrickGameMode_RestartGame_Offset;
            using fn = void (__thiscall *)(void * ABrickGameMode);
            fn func = reinterpret_cast<fn>(restartGameFunctionAdr);
            func(activeABrickGameMode->GetCurrentAdr());
            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/restart/allPlayers")([]() {
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode)) {
            auto restartAllPlayersFunctionAdr = (uintptr_t) GetModuleHandleA(NULL) + ABrickGameMode_RestartAllPlayers_Offset;
            using fn = void (__thiscall *)(void * ABrickGameMode, bool thing);
            fn func = reinterpret_cast<fn>(restartAllPlayersFunctionAdr);
            func(activeABrickGameMode->GetCurrentAdr(), true);
            // TODO: Unclear what the second argument does; seems to do nothing, but cannot confirm.
            // Please check.
            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });


    // Info
    CROW_ROUTE(app, "/")([]() {
        crow::json::wvalue response;
        response["/screenshot"] = "Take a screenshot.";
        response["/kill"] = "Quit Brick Rigs, forcefully.";
        response["/ABrickGameMode"] = "Visit this endpoint for more information.";
        return response;
    });

    // Get a screenshot
    CROW_ROUTE(app, "/screenshot")
    ([](){
        crow::response rsp;
        rsp.set_header("Content-Type", "mime/png");

        time_t timestamp;
        time(&timestamp);
        auto scn_name = "attachment; filename=\"BRLMTScreenshot" + std::string(ctime(&timestamp) + std::string("\""));
        rsp.set_header("Content-Disposition", scn_name);

        auto scn = GetScreenshot();
        std::string png(scn.begin(), scn.end());

        rsp.write(png);

        return rsp;
    });

    // Quit the game forcefully.
    CROW_ROUTE(app, "/kill")
    ([](){
        ExitProcess(0);
        return crow::response(200);
    });

    app
        .port(BINDPORT)
        .loglevel(crow::LogLevel::Debug)
        //.multithreaded() // Don't enable multithreaded, unsure what will happen with race conditions.
        .run();
}


// Required Win32 Signature; don't modify
bool __stdcall DllMain(void *, std::uint32_t reason, void *) {
    if (reason == DLL_PROCESS_ATTACH) {
        AllocConsole();
        freopen("CONOUT$","w", stdout);
        freopen("CONOUT$","w", stderr);

        std::cout << "Trying to start HTTP Server..." << std::endl;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Run, NULL, 0, NULL);
        return true;
    } else {
    return false;
    }
}
