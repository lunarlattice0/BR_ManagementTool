#include "payload.hpp"
#include "crow/common.h"
#include "crow/http_response.h"
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
    this->GWorldAdr = *reinterpret_cast<void**>((uintptr_t)GetModuleHandle(NULL) + GWorldOffset);
}

void* GWorld::GetCurrentAdr() {
    return this->GWorldAdr;
}


ABrickGameMode::ABrickGameMode(void* gworld) {
    auto getFunctionAdr = (uintptr_t)GetModuleHandle(NULL) + ABrickGameMode_Get_Offset;
    using getfn = void* (__cdecl *) (void* UObject);
    getfn get = reinterpret_cast<getfn>(getFunctionAdr);
    this->ABrickGameModeAdr = reinterpret_cast<void*>(get(gworld));
}

// Main function
void Run() {
    // Calculate UWorld offset
    std::unique_ptr<GWorld> gworld = std::make_unique<GWorld>();

    crow::SimpleApp app;

    // Debug: Get Abrickgamemode adr
    CROW_ROUTE(app, "/get")([&](){
        std::unique_ptr<ABrickGameMode> current_gamemode = std::make_unique<ABrickGameMode>(gworld->GetCurrentAdr());
        return crow::response(200);
    });

    CROW_ROUTE(app, "/")([]() {
        return "Welcome to a BRLMT Server!";
    });

    // Get a screenshot
    CROW_ROUTE(app, "/screenshot").methods(crow::HTTPMethod::GET)
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

    // Quit the game.
    CROW_ROUTE(app, "/kill").methods(crow::HTTPMethod::GET)
    ([](){
        ExitProcess(0);
        return crow::response(200);
    });

    // Get the base address (DEBUG)
    CROW_ROUTE(app, "/base_adr").methods(crow::HTTPMethod::GET)
    ([]() {
        uintptr_t base_adr = (uintptr_t)GetModuleHandle(NULL);
        return std::to_string(base_adr);
    });

    app
        .port(BINDPORT)
        .loglevel(crow::LogLevel::Debug)
        .multithreaded()
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
