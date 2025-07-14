#include "payload.hpp"
#include "crow/logging.h"
#include <consoleapi.h>
#include <windows.h>
#include <winnt.h>
#include <winuser.h>

#ifndef BINDADDR
#define BINDADDR "0.0.0.0"
#endif

#ifndef BINDPORT
#define BINDPORT 8080
#endif

void Run() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api")([](){
        return "Hello wrold";
    });

    app
        .port(BINDPORT)
        .loglevel(crow::LogLevel::Debug)
        .multithreaded()
        .run();
}

bool __stdcall DllMain(void *, std::uint32_t reason, void *) {
    if (reason == DLL_PROCESS_ATTACH) {
        AllocConsole();
        freopen("CONOUT$","w", stdout);
        freopen("CONOUT$","w", stderr);

        std::cout << "Trying to start HTTP Server..." << std::endl;
        Run();
        return true;
    } else {
    return false;
    }
}
