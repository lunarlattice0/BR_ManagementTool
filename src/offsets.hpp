#pragma once

#include <cstdint>
#include <memory>

// Offsets
constexpr uintptr_t GWorld_Offset = 0x4b3f258;
constexpr uintptr_t GWorld_PlayerControllerList_Offset = GWorld_Offset + 0x1c0;
constexpr uintptr_t ABrickGameMode_Get_Offset = 0x140dc6cc0;
constexpr uintptr_t ABrickGameMode_EndMatch_Offset = 0x0dc4800;
constexpr uintptr_t ABrickGameMode_RestartGame_Offset = 0x0de63b0;
constexpr uintptr_t ABrickGameMode_RestartAllPlayers_Offset = 0x0de62f0;
constexpr uintptr_t ABrickGameMode_EndRound_Offset = 0x0dc49a0;
constexpr uintptr_t ABrickPlayerController_AdminSay_Offset = 0x0df9990; // (MANUAL MATCH)
constexpr uintptr_t ABrickPlayerController_KillCharacter_Offset = 0x0e11f60;
constexpr uintptr_t ABrickPlayerController_GetPlayerName_Offset = 0x0e09b20;
constexpr uintptr_t ABrickGameSession_Get_Offset = 0x0e05c90;
constexpr uintptr_t ABrickGameSession_KickPlayer_Offset = 0x255f850; // (MANUAL MATCH)
constexpr uintptr_t ABrickGameSession_BanPlayer_Offset = 0x0e11990;
constexpr uintptr_t ABrickVehicle_GetSpawningPlayerControllerAdr_Offset =  0x0f125e0;

constexpr uintptr_t ABrickVehicle_EjectAllCharacters_Offset = 0x0f09490;
constexpr uintptr_t ABrickVehicle_ScrapVehicle_Offset = 0x0e20b20;

constexpr uintptr_t FTextFromFString_Offset = 0x105b120;
constexpr uintptr_t GetPlayerUniqueNetId_Offset = 0x0d7f080;
constexpr uintptr_t FTextToFString_Offset = 0x1072b10;

constexpr uintptr_t ABrickPlayerControllerHookCtorAdr_Offset = 0x0df4030;
constexpr uintptr_t ABrickPlayerControllerHookDtorAdr_Offset = 0x0df65b0;
constexpr uintptr_t ABrickVehicleControllerHookCtorAdr_Offset = 0x0f01680; // NOTE: this hooks the BeginPlay(), not the ctor. (MANUAL MATCH)
constexpr uintptr_t ABrickVehicleControllerHookDtorAdr_Offset = 0x0f093c0; // NOTE: this hooks the Destroyed(), not the dtor.


constexpr uintptr_t FBrickChatMessageCtor_Offset = 0x0df4fe0;
constexpr uintptr_t FBrickChatMessageGetMessageText_Offset = 0x0e08430;
