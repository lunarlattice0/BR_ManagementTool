// Research https://www.unknowncheats.me/forum/unreal-engine-4-a/697780-call-private-functions-uobjects.html

#pragma once

typedef unsigned char EChatMessageType;
constexpr EChatMessageType None = 0x00;
constexpr EChatMessageType Message = 0x01;
constexpr EChatMessageType Join = 0x02;
constexpr EChatMessageType Leave = 0x03;
constexpr EChatMessageType Kick = 0x04;
constexpr EChatMessageType JoinBanned = 0x05;
constexpr EChatMessageType Unban = 0x06;
constexpr EChatMessageType Death = 0x07;
constexpr EChatMessageType MatchSettings = 0x08;
constexpr EChatMessageType VehicleSpawnAttempt = 0x09;
constexpr EChatMessageType VehicleSpawnSuccess = 0xa;
constexpr EChatMessageType VehicleSpawnFailure = 0xb;

struct FChatMessagePlayerInfoFrom {
    // FUniqueNetIdRepl not implemented
    wchar_t player; // you better pray this is not platform specific
    // Strategy: Check for \0 at the end.
};

struct FChatMessagePlayerInfoTo {
    // FUniqueNetIdRepl not implemented
    wchar_t player; // you better pray this is not platform specific
    // Strategy: Check for \0 at the end.
};

// Note, i'm not too sure what the difference between the two is. Perhaps whisper chat? unsure.
struct FText {
    // WTF.
};

typedef unsigned char FGenericTeamId;
// FFluUGCItemID not implemented

class FBrickChatMessage {
    private:
        EChatMessageType messageType;
        FChatMessagePlayerInfoFrom fromPlayer;
        FChatMessagePlayerInfoTo toPlayer;
        FText text;
        FGenericTeamId teamId;
        // FFluUGCItemID not implemented
    public:
        FBrickChatMessage(EChatMessageType messageType, FChatMessagePlayerInfoFrom fromPlayer, FChatMessagePlayerInfoTo toPlayer, FText text, FGenericTeamId teamId);
        // Structs self destruct, so use default dtor
};
