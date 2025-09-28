#pragma once

#include <cstdint>
#include <memory>

// Offsets
constexpr uintptr_t GObjects_Offset = 0x043d2600;
constexpr uintptr_t GWorld_Offset = 0x04515F18;
constexpr uintptr_t GWorld_PlayerControllerList_Offset = GWorld_Offset + 0x1c0;
constexpr uintptr_t ABrickGameMode_Get_Offset = 0x0ce0460;
constexpr uintptr_t ABrickGameMode_EndMatch_Offset = 0x0cde190;
constexpr uintptr_t ABrickGameMode_RestartGame_Offset = 0x0cffc20;
constexpr uintptr_t ABrickGameMode_RestartAllPlayers_Offset = 0x0cffb60;
constexpr uintptr_t ABrickGameMode_EndRound_Offset = 0x0cde330;
constexpr uintptr_t ABrickPlayerController_AdminSay_Offset = 0x0d139a0;
constexpr uintptr_t ABrickPlayerController_KillCharacter_Offset = 0x0d2da90;
constexpr uintptr_t ABrickPlayerController_GetPlayerName_Offset = 0x0d25fd0;
constexpr uintptr_t ABrickGameSession_Get_Offset = 0x0d20e10;
constexpr uintptr_t ABrickGameSession_KickPlayer_Offset = 0x0d2d4b0;
constexpr uintptr_t ABrickGameSession_BanPlayer_Offset = 0x0d2d4c0;
constexpr uintptr_t ABrickVehicle_GetSpawningPlayerControllerAdr_Offset =  0x0e14160;
constexpr uintptr_t ABrickVehicle_ScrapVehicle_Offset = 0x0e21820;

constexpr uintptr_t FTextFromFString_Offset = 0x0f53290;
constexpr uintptr_t GetPlayerUniqueNetId_Offset = 0x0c9a5c0;
constexpr uintptr_t FTextToFString_Offset = 0x0f69af0;

constexpr uintptr_t ABrickPlayerControllerHookCtorAdr_Offset = 0x0d0d930;
constexpr uintptr_t ABrickPlayerControllerHookDtorAdr_Offset = 0x0d0ff30;
constexpr uintptr_t ABrickVehicleControllerHookCtorAdr_Offset = 0x0e03220; // NOTE: this hooks the BeginPlay(), not the ctor.
constexpr uintptr_t ABrickVehicleControllerHookDtorAdr_Offset = 0x0e0ae50; // NOTE: this hooks the Destroyed(), not the dtor.

constexpr uintptr_t FBrickChatMessageCtor_Offset = 0x0d0e8e0;
constexpr uintptr_t FBrickChatMessageGetMessageText_Offset = 0x0d24820;
