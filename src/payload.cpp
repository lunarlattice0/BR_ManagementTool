#include "payload.hpp"
#include "MinHook.h"
#include "crow/common.h"
#include "crow/http_server.h"
#include "crow/json.h"
#include <codecvt>
#include <consoleapi.h>
#include <cstdint>
#include <iostream>
#include <libloaderapi.h>
#include <memoryapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <windows.h>
#include <winnt.h>
#include <winscard.h>
#include <crow.h>
#include <gdiplus.h>
#include <gdiplus/gdiplusheaders.h>
#include <ctime>
#include "offsets.hpp"
#include "utypes/utypes.hpp"

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
std::shared_ptr<ABrickGameSession> activeABrickGameSession;

std::vector<uintptr_t> ABrickPlayerControllerList;
std::vector<uintptr_t> ABrickVehicleList;
std::vector<uintptr_t> ChatMessageList;

// macros for calling funcs
template <typename returnType, typename fn_sig, typename...args>
static inline returnType CALL_BR_FUNC_WITH_RETURN(uintptr_t offset, args... arguments) {
    auto funcAdr = (uintptr_t)GetModuleHandleA(NULL) + offset;
    using fn = fn_sig;
    fn br_func = reinterpret_cast<fn>(funcAdr);
    return br_func(arguments...);
}

template <typename fn_sig, typename...args>
static inline void CALL_BR_FUNC(uintptr_t offset, args... arguments) {
    auto funcAdr = (uintptr_t)GetModuleHandleA(NULL) + offset;
    using fn = fn_sig;
    fn br_func = reinterpret_cast<fn>(funcAdr);
    br_func(arguments...);
    return;
}

// Refresh globalvars
inline void refreshGlobals() {
    activeGWorld = std::make_shared<GWorld>();
    activeABrickGameMode = std::make_shared<ABrickGameMode>(activeGWorld->GetCurrentAdr());
    activeABrickGameSession = std::make_shared<ABrickGameSession>(activeGWorld->GetCurrentAdr());
};

inline FText * GetFTextFromFString (FString * fstr_in) {
    FText * ft = new FText;
    CALL_BR_FUNC<void (__thiscall *)(FText *, FString *)>(FTextFromFString_Offset, ft, fstr_in);

    return ft;
}

inline FString * GetFStringFromFText(FText * ftxt) {
    return CALL_BR_FUNC_WITH_RETURN<FString *, FString *(__thiscall *)(FText *)>(FTextToFString_Offset, ftxt);
};

inline FUniqueNetIdRepl * GetPlayerUniqueNetId (std::uintptr_t abpc) {
    refreshGlobals();
    if (InternalClassExists(activeABrickGameMode)) {
        FUniqueNetIdRepl * funir = new FUniqueNetIdRepl;

        CALL_BR_FUNC<void (__cdecl*)(FUniqueNetIdRepl * netid, void * abpc)>(GetPlayerUniqueNetId_Offset, funir, (void*)abpc);
        return funir;

    } else {
        return nullptr;
    }
}

// https://stackoverflow.com/a/557774
HMODULE GetCurrentModule() {
    HMODULE hModule=NULL;
    GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        (LPCTSTR)GetCurrentModule,
        &hModule);
    return hModule;

}

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
    this->gworldptr = *reinterpret_cast<void**>((uintptr_t)GetModuleHandle(NULL) + GWorld_Offset);
}

void* GWorld::GetCurrentAdr() {
    return this->gworldptr;
}

ABrickGameMode::ABrickGameMode(void* gworld) {
    this->gworldptr = gworld;
}

APlayerController::APlayerController(void * ptr) {
    this->gworldptr = ptr;
}

ABrickGameSession::ABrickGameSession(void * gworld) {
    this->gworldptr = gworld;
}

void * APlayerController::GetCurrentAdr() { // SUPER DUPER CAUTION
    return this->gworldptr;
}

void * ABrickGameMode::GetCurrentAdr() {
    return CALL_BR_FUNC_WITH_RETURN<void*, void* (__cdecl *) (void* UObject)>(ABrickGameMode_Get_Offset, this->gworldptr);

}

void * ABrickGameSession::GetCurrentAdr() {
    return CALL_BR_FUNC_WITH_RETURN<void*, void*(__cdecl*)(void * UObject)>(ABrickGameSession_Get_Offset, this->gworldptr);
}


// Hooks
void * abvhfc_ptr;
void ABrickVehicleHookFuncCTOR() {
    uintptr_t rcx = 0;
    asm volatile (
        "":
        "=c"(rcx)
        ::
    );
    ABrickVehicleList.push_back(rcx);

    using fn = void (__thiscall*)(void * abv);
    fn func = reinterpret_cast<fn>(abvhfc_ptr);
    func((void*)rcx);
}

// BrickPlayerController
void * abpchfc_ptr;
void ABrickPlayerControllerHookFuncCTOR() {
    uintptr_t rcx = 0;
    asm volatile (
        "":
        "=c"(rcx)
        ::
    );
    ABrickPlayerControllerList.push_back(rcx);

    using fn = void (__thiscall *)(void* abpc);
    fn func = reinterpret_cast<fn>(abpchfc_ptr);
    func((void*)rcx);
}

void * acm_ptr;
void AddChatMessageHookFunc() {
    uintptr_t rcx = 0;
    uintptr_t rdx = 0;
    asm volatile (
        "":
        "=c"(rcx), "=d"(rdx)
        ::
    );
    ChatMessageList.push_back(rdx);

    using fn = void(__thiscall *)(void * abgs, void* fbcm);
    fn func = reinterpret_cast<fn>(acm_ptr);
    func((void*)rcx, (void*)rdx);
    std::cout << "AddChatMessage called: " << rdx << std::endl;
    // TODO: Figure out why rdx is the same every time
}

void * abpchfd_ptr;
void ABrickPlayerControllerHookFuncDTOR() {
    uintptr_t rcx = 0;
    // Read RCX register
    asm volatile (
        "":
        "=c"(rcx)
        :
        :
    );
    ABrickPlayerControllerList.erase(std::remove(ABrickPlayerControllerList.begin(), ABrickPlayerControllerList.end(), rcx), ABrickPlayerControllerList.end());

    using fn = void (__thiscall *)(void* abpc);
    fn func = reinterpret_cast<fn>(abpchfd_ptr);
    func((void*)rcx);
}

void * abvhfd_ptr;
void ABrickVehicleHookFuncDTOR() {
    uintptr_t rcx = 0;
    asm volatile (
        "":
        "=c"(rcx)
        ::
    );
    ABrickVehicleList.erase(std::remove(ABrickVehicleList.begin(), ABrickVehicleList.end(), rcx), ABrickVehicleList.end());

    using fn = void(__thiscall*)(void * abv);
    fn func = reinterpret_cast<fn>(abvhfd_ptr);
    func((void*)rcx);
}

uintptr_t ABrickPlayerControllerHookCtorAdr = (uintptr_t) GetModuleHandleA(NULL) + ABrickPlayerControllerHookCtorAdr_Offset;
uintptr_t ABrickPlayerControllerHookDtorAdr = (uintptr_t) GetModuleHandleA(NULL) + ABrickPlayerControllerHookDtorAdr_Offset;

uintptr_t ABrickVehicleControllerHookCtorAdr = (uintptr_t) GetModuleHandleA(NULL) + ABrickVehicleControllerHookCtorAdr_Offset;
uintptr_t ABrickVehicleControllerHookDtorAdr = (uintptr_t) GetModuleHandleA(NULL) + ABrickVehicleControllerHookDtorAdr_Offset;

uintptr_t AddChatMessageAdr = (uintptr_t) GetModuleHandleA(NULL) + AddChatMessageAdr_Offset;

// Main function
void Run() {
    MH_Initialize();
    // Hook ABrickPlayerController ctor and dtor
    MH_CreateHook((void*)ABrickPlayerControllerHookCtorAdr, (void*)ABrickPlayerControllerHookFuncCTOR, &abpchfc_ptr);
    MH_EnableHook((void*)ABrickPlayerControllerHookCtorAdr);

    MH_CreateHook((void*)ABrickPlayerControllerHookDtorAdr, (void*)ABrickPlayerControllerHookFuncDTOR, &abpchfd_ptr);
    MH_EnableHook((void*)ABrickPlayerControllerHookDtorAdr);

    // Hook ABrickVehicle ctor and dtor
    MH_CreateHook((void*)ABrickVehicleControllerHookCtorAdr, (void*)ABrickVehicleHookFuncCTOR, &abvhfc_ptr);
    MH_EnableHook((void*)ABrickVehicleControllerHookCtorAdr);

    MH_CreateHook((void*)ABrickVehicleControllerHookDtorAdr, (void*)ABrickVehicleHookFuncDTOR, &abvhfd_ptr);
    MH_EnableHook((void*)ABrickVehicleControllerHookDtorAdr);

    // Hook ServerSendChatMessage
    MH_CreateHook((void*) AddChatMessageAdr, (void*)AddChatMessageHookFunc, &acm_ptr);
    MH_EnableHook((void*) AddChatMessageAdr);

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
        response["/remove/messages"] = "Clear server message buffer.";
        response["/get/messages"] = "Return all sent messages on the server.";
        response["/adminSay"] = "Say something as the admin. Include the message as the body of the request (JSON not required).";
        response["/restart/allPlayers"] = "Restart all players (including dead players) to spawn.";
        response["/get/allPlayers"] = "Return a JSON of all players, in the format {memory address, name}.";
        response["/kill/<player>"] = "Kill a player, using the memory address of the player as an endpoint.";
        response["/scrap/<vehicle>"] = "Remove a vehicle, using the memory address of the vehicle as an endpoint.";
        response["/get/allVehicles"] = "Get information of all vehicles.";
        response["/kick/<player>"] = "Kick a player, using the memory address of the player as an endpoint. Include the kick message as the body of the request (JSON not required).";
        response["/ban/<player>/<duration in seconds>"] = "Ban a player, using the memory address of the player and duration as an endpoint. Include the ban message as the body of the request (JSON not required).";
        //response["/unban/<player>"] = "Unban a player. Not implemented. TODO"
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

    CROW_ROUTE(app, "/ABrickGameMode/kick/<uint>").methods(crow::HTTPMethod::Post)([](const crow::request& req, std::uintptr_t memory_adr) {
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode) && InternalClassExists(activeABrickGameSession)) {
            if (std::find(ABrickPlayerControllerList.begin(), ABrickPlayerControllerList.end(), memory_adr) == ABrickPlayerControllerList.end()) {
                throw std::out_of_range("Player not found");
            }

            using convert_type = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<convert_type, wchar_t> converter;
            std::wstring kickStr = converter.from_bytes(req.body);

            FString * fstr = new FString(kickStr.c_str());
            FText * ftxt = GetFTextFromFString(fstr);

            bool success = CALL_BR_FUNC_WITH_RETURN<bool, bool(__thiscall*)(void*ABrickGameSession, void * abpc, void * ftxt)>(
                ABrickGameSession_KickPlayer_Offset,
                activeABrickGameSession->GetCurrentAdr(),
                (void*)memory_adr, ftxt
            );

            // crappy hack to avoid the "INVALID PLAYER" glitch
            ABrickPlayerControllerList.erase(std::remove(ABrickPlayerControllerList.begin(), ABrickPlayerControllerList.end(), memory_adr), ABrickPlayerControllerList.end());

            delete ftxt;
            delete fstr;
            return success ? crow::response(200) : crow::response(500);

        } else {
            return crow::response(503);
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/get/allVehicles").methods(crow::HTTPMethod::Get)([](){
            refreshGlobals();

            if (InternalClassExists(activeABrickGameMode)) {
                crow::json::wvalue response;

                std::vector<crow::json::wvalue> listOfVehicles;

                for (uintptr_t brickVehicle : ABrickVehicleList) {

                    auto abpc = CALL_BR_FUNC_WITH_RETURN<void*, void * (__thiscall *)(void * ABrickVehicle)>(ABrickVehicle_GetSpawningPlayerControllerAdr_Offset,
                        (void*)brickVehicle);

                    // set up wstring converter
                    using convert_type = std::codecvt_utf8<wchar_t>;
                    std::wstring_convert<convert_type, wchar_t> converter;

                    // Figure out vehicle's name
                    // FUGCFileInfo @ brickVehicle + 0x378
                    uintptr_t fugcfileinfo = brickVehicle + 0x378;
                    // Title @ FUGCFileInfo + 0x50;
                    auto vehicleTitleVoidPtr = (void*)(fugcfileinfo + 0x50);
                    FString * vehicleTitle = (FString *)(vehicleTitleVoidPtr);

                    /*
                    // Figure out vehicle's steamid
                    // OnlineItemId @ FUGCFileInfo + 0x20
                    auto vehicleOnlineItemIdPtr = (fugcfileinfo + 0x20);
                    auto vehicleSteamItemIdPtr = (void*)(vehicleOnlineItemIdPtr + )

                    ULONG64 steamItemId = (ULONG64)(vehicl)
                    */



                    std::pair<std::string, crow::json::wvalue> vehicle_adr("vehicle_memory_adr", std::to_string(brickVehicle));
                    std::pair<std::string, crow::json::wvalue> memory_adr("spawning_player_adr", std::to_string((uintptr_t)abpc));
                    std::pair<std::string, crow::json::wvalue> name("vehicle_name", converter.to_bytes(vehicleTitle->ToWString()));
                    //std::pair<std::string, crow::json::wvalue> url("vehicle_hash", converter.to_bytes(vehicleLocalItemId->ToWString()));
                    // TO BE IMPLEMENTED.

                    crow::json::wvalue value{vehicle_adr, memory_adr, name}; //, url};
                    listOfVehicles.push_back(value);
                }

                response["vehicles"] = crow::json::wvalue(listOfVehicles);
                return response;
            } else {
                return crow::json::wvalue();
            }
        });


    CROW_ROUTE(app, "/ABrickGameMode/ban/<uint>/<uint>").methods(crow::HTTPMethod::Post)([](const crow::request& req, std::uintptr_t memory_adr, unsigned int seconds) {
            refreshGlobals();
            if (InternalClassExists(activeABrickGameMode) && InternalClassExists(activeABrickGameSession)) {
                if (std::find(ABrickPlayerControllerList.begin(), ABrickPlayerControllerList.end(), memory_adr) == ABrickPlayerControllerList.end()) {
                    throw std::out_of_range("Player not found");
                }

                auto playerFunir = GetPlayerUniqueNetId(memory_adr);
                FTimespan ft;

                // 1 second = 10000000 ticks
                ft.ticks = seconds * 10000000;

                // TODO: fstr is actually playername...fix that eventually.
                FString * fstr = new FString(L"");

                using convert_type = std::codecvt_utf8<wchar_t>;
                std::wstring_convert<convert_type, wchar_t> converter;
                std::wstring kickStr = converter.from_bytes(req.body);
                FString * reason = new FString(kickStr.c_str());

                CALL_BR_FUNC<void (__thiscall*)(void * ABrickGameSession, FUniqueNetIdRepl * playerFunir, FString * name, FString * reason, FTimespan * ticks)>(
                    ABrickGameSession_BanPlayer_Offset,

                    activeABrickGameSession->GetCurrentAdr(),
                    playerFunir,
                    fstr,
                    reason,
                    &ft
                    );

                // crappy hack to avoid the "INVALID PLAYER" glitch
                ABrickPlayerControllerList.erase(std::remove(ABrickPlayerControllerList.begin(), ABrickPlayerControllerList.end(), memory_adr), ABrickPlayerControllerList.end());

                delete fstr;
                delete playerFunir;
                return crow::response(200);
            } else {
                return crow::response(503);
            }
        });

    CROW_ROUTE(app, "/ABrickGameMode/get/messages").methods(crow::HTTPMethod::Get)([](){
            refreshGlobals();
            if (InternalClassExists(activeABrickGameMode)) {
                for (auto messageAdr : ChatMessageList) {
                    std::cout << messageAdr << std::endl;
                    auto messagePtr = static_cast<FBrickChatMessage *>((void*)messageAdr);
                    std::cout << messagePtr->messageType << std::endl;

                }

                return crow::json::wvalue();
            } else {
                return crow::json::wvalue();
            }
        });

    CROW_ROUTE(app, "/ABrickGameMode/remove/messages").methods(crow::HTTPMethod::Post)([](){
            refreshGlobals();
            if (InternalClassExists(activeABrickGameMode)) {
                ChatMessageList.clear();
                return crow::response(200);
            } else {
                return crow::response(503);
            }
        });

    CROW_ROUTE(app, "/ABrickGameMode/get/allPlayers").methods(crow::HTTPMethod::Get)([](){
        refreshGlobals();

        if (InternalClassExists(activeABrickGameMode)) {
            crow::json::wvalue response;

            std::vector<crow::json::wvalue> listOfPlayers;

            for (uintptr_t playerController : ABrickPlayerControllerList) {
                FString * fstr = new FString(L"");
                CALL_BR_FUNC<void (__thiscall *)(void * abpc, FString * fstr_ptr)>(ABrickPlayerController_GetPlayerName_Offset,
                    (void*)playerController, fstr);

                // https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
                using convert_type = std::codecvt_utf8<wchar_t>;
                std::wstring_convert<convert_type, wchar_t> converter;
                // Consider bumping min C++ to 23? Unicode support?

                // TODO: Get steamid
                std::pair<std::string, crow::json::wvalue> name("name", converter.to_bytes(fstr->ToWString()));
                std::pair<std::string, crow::json::wvalue> memory_adr("memory_adr", std::to_string(playerController));
                crow::json::wvalue value{name, memory_adr};
                listOfPlayers.push_back(value);
                delete(fstr);
            }

            response["players"] = crow::json::wvalue(listOfPlayers);
            return response;
        } else {
            return crow::json::wvalue();
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/scrap/<uint>")([](uintptr_t memory_adr){
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode)) {
            if (std::find(ABrickVehicleList.begin(), ABrickVehicleList.end(), memory_adr) == ABrickVehicleList.end()) {
                throw std::out_of_range("Vehicle not found");
            }

            CALL_BR_FUNC<void(__thiscall*)(void*abpc)>(ABrickVehicle_ScrapVehicle_Offset, (void*)memory_adr);

            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/kill/<uint>")([](uintptr_t memory_adr){
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode)) {
            if (std::find(ABrickPlayerControllerList.begin(), ABrickPlayerControllerList.end(), memory_adr) == ABrickPlayerControllerList.end()) {
                throw std::out_of_range("Player not found");
            }


            CALL_BR_FUNC<void(__thiscall*)(void*abpc)>(ABrickPlayerController_KillCharacter_Offset, (void*)memory_adr);

            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    // End a match
    CROW_ROUTE(app, "/ABrickGameMode/end/match")([&](){
        refreshGlobals();

        if (InternalClassExists(activeABrickGameMode)) {

            CALL_BR_FUNC<void (__thiscall *)(void * ABrickGameMode)>(ABrickGameMode_EndMatch_Offset, activeABrickGameMode->GetCurrentAdr());
            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/end/round")([&](){
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode)) {
            // Use a fake team struct
            typedef struct {
                unsigned char TeamID;
            }__attribute__((packed, aligned(1))) stubbedTeam;
            stubbedTeam * fakeTeam = new stubbedTeam{};
            fakeTeam->TeamID = 0;
            //func(activeABrickGameMode->GetCurrentAdr(), &fakeTeam, false);
            CALL_BR_FUNC<void(__thiscall*)(void*ABrickGameMode,void*matchWinner,bool lastRound)>(
                ABrickGameMode_EndRound_Offset, activeABrickGameMode->GetCurrentAdr(), &fakeTeam, false);
            delete(fakeTeam);
            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/restart/game")([&]() {
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode)) {
            CALL_BR_FUNC<void (__thiscall *)(void * ABrickGameMode)>(ABrickGameMode_RestartGame_Offset, activeABrickGameMode->GetCurrentAdr());
            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/adminSay").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode)) {

            uintptr_t adminPtr = ABrickPlayerControllerList.at(0);

            // convert str to wstr
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wide = converter.from_bytes(req.body);

            FString * fstr = new FString(wide.c_str());

            auto ftxt = GetFTextFromFString(fstr);

            CALL_BR_FUNC<void(__thiscall*)(void*abpc, FText*)>(ABrickPlayerController_AdminSay_Offset, (void*)adminPtr, ftxt);

            delete ftxt;
            delete fstr;
            return crow::response(200);
        } else {
            return crow::response(503);
        }
    });

    CROW_ROUTE(app, "/ABrickGameMode/restart/allPlayers")([]() {
        refreshGlobals();
        if (InternalClassExists(activeABrickGameMode)) {
            CALL_BR_FUNC<void(__thiscall*)(void*ABrickGameMode, bool thing)>(ABrickGameMode_RestartAllPlayers_Offset, activeABrickGameMode->GetCurrentAdr(), true);
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
        //response["/start"] = "Start a server with default settings.";
        return response;
    });

    /*  TODO: implement.
    CROW_ROUTE(app, "/start")([](){
        ABrickGameSessionStruct * abgs = new ABrickGameSessionStruct;
        auto abgsCtor = (uintptr_t) GetModuleHandleA(NULL) + 0x0d0d880;
        using fn = void (__thiscall *)(void * abgs);
        fn func = reinterpret_cast<fn>(abgsCtor);
        func(abgs);
        return crow::response(200);
    });
    */

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

    CROW_ROUTE(app, "/detach")
    ([](){
        MH_DisableHook(MH_ALL_HOOKS);
        FreeLibraryAndExitThread(GetCurrentModule(), 0);
        return crow::response(200);
    });

    // Quit the game forcefully.
    CROW_ROUTE(app, "/kill")
    ([](){
        ExitProcess(0);
        return crow::response(200);
    });

    // DEBUG: Dump the memory of a certain length.
    CROW_ROUTE(app, "/memory").methods(crow::HTTPMethod::Get)([](const crow::request& req){
        auto address = req.url_params.get("address");
        auto length = req.url_params.get("length");
        crow::response rsp;

        if (!address || !length) {
            return crow::response(400);
        }

        uintptr_t addressull = strtoull(address, NULL, 0);
        unsigned long long lengthull = strtoull(length, NULL, 0);

        std::stringstream memdump;
        for (uintptr_t p = addressull; p < addressull + lengthull; p += sizeof(char)) {
            memdump << std::hex << (0xFF & *(char*)p) << " ";
        }

        memdump << std::endl;
        auto memdumpStr = memdump.str();
        rsp.write(memdumpStr.c_str());
        rsp.set_header("Content-Disposition", "inline");
        return rsp;
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
