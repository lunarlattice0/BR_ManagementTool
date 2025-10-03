// A trimmed down reimplementation of UnrealContainers.
// Modified from BRSD, under MIT license, copyright Aaron Wilk.

/*
 MIT License

 Copyright (c) 2025 Aaron Wilk

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#pragma once
#include <iostream>

template<typename ArrayElementType>
class TArray {
    protected:
        static constexpr uint64_t ElementAlign = alignof(ArrayElementType);
        static constexpr uint64_t ElementSize = sizeof(ArrayElementType);
        ArrayElementType* Data;
        int32_t NumElements;
        int32_t MaxElements;
    public:
        TArray() : Data(nullptr), NumElements(0), MaxElements(0) {}
        bool Add(const ArrayElementType& Element) {
            if (GetSlack() <= 0)
                return false;
            Data[NumElements] = Element;
            NumElements++;
            return true;
        }
        bool Remove(int32_t Index) {
            if (!IsValidIndex(Index))
                return false;

            NumElements--;
            for(int i = Index; i < NumElements; i++) {
                Data[i] = Data[i+1];
            }
            return true;
        }
        inline int32_t Num() const {return NumElements;}
        inline int32_t Max() const {return MaxElements;}
        inline bool IsValidIndex(int32_t Index) const { return Data && Index >= 0 && Index < NumElements;}
        inline bool IsValid() const {return Data && NumElements > 0 && MaxElements >= NumElements;}
        inline ArrayElementType& operator[](int32_t Index) {VerifyIndex(Index); return Data[Index];}
        inline const ArrayElementType& operator[](int32_t Index) const {VerifyIndex(Index); return Data[Index];}
        inline bool operator==(const TArray<ArrayElementType>& Other) const {return Data == Other.Data;}
        inline bool operator!=(const TArray<ArrayElementType>& Other) const { return Data != Other.Data;}
        inline explicit operator bool() const {return IsValid();};

    private:
        inline void VerifyIndex(int32_t Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index out of range");}
        inline int32_t GetSlack() const {return MaxElements-NumElements;}
        inline ArrayElementType& GetUnsafe(int32_t Index) {return Data[Index];}
        inline const ArrayElementType& GetUnsafe(int32_t Index) const {return Data[Index];}
};

class FString : public TArray <wchar_t> {
    public:
        // friend std::ostream& operator << (std::ostream& Stream, const FString& Str) { return Stream << Str.ToString(); }
        FString(const wchar_t* Str) {
            const uint32_t NullTerminatedLength = static_cast<uint32_t>(wcslen(Str) + 0x1);
            Data = const_cast<wchar_t*>(Str);
            NumElements = NullTerminatedLength;
            MaxElements = NullTerminatedLength;
        }
        FString() {FString(L"");};
        inline std::wstring ToWString() const {
            if (*this)
                return std::wstring(Data);

            return L"";
        }
        //inline wchar_t* CStr() {return Data;}
        //inline const wchar_t* CStr() const {return Data;}

};

struct __attribute__((packed, aligned(8))) FText {
    uint8_t text_data[0x10];
    uint32_t flags;
};

static_assert(alignof(FText) == 0x000008, "Wrong alignment on FText");
static_assert(sizeof(FText) == 0x000018, "Wrong size on FText");
static_assert(offsetof(FText, text_data) == 0x000000, "Member 'FText::TextData' has a wrong offset!");

// No actual implementation.
struct __attribute__((packed, aligned(0x1))) FUniqueNetIdRepl {
    uint8_t data[0x28];
};
static_assert(alignof(FUniqueNetIdRepl) == 0x1, "Wrong alignment of FUniqueNetIdRepl");
static_assert(sizeof(FUniqueNetIdRepl) == 0x28, "Wrong size of FUniqueNetIdRepl");

struct FTimespan {
    int64_t ticks;
};
static_assert(alignof(FTimespan) == 0x8, "Wrong alignment of FUniqueNetIdRepl");
static_assert(sizeof(FTimespan) == 0x8, "Wrong size of FUniqueNetIdRepl");

enum EChatMessageType {
    None = 0x0,
    Message = 0x1,
    Join = 0x2,
    Leave = 0x3,
    Kick = 0x4,
    JoinBanned = 0x5,
    Unban = 0x6,
    Death = 0x7,
    MatchSettings = 0x8,
    VehicleSpawnAttempt = 0x9,
    VehicleSpawnSuccess = 0xa,
    VehicleSpawnFailure = 0xb,
};

struct FChatMessagePlayerInfo {
    FUniqueNetIdRepl playerid;
    FString playername;
};
static_assert(alignof(FChatMessagePlayerInfo) == 0x8, "Wrong alignment of FChatMessagePlayerInfo");
static_assert(sizeof(FChatMessagePlayerInfo) == 0x38, "Wrong size of FChatMessagePlayerInfo");

struct __attribute((aligned(0x8))) FBrickChatMessage {
    EChatMessageType messageType;
    FChatMessagePlayerInfo sourcePlayer;
    FChatMessagePlayerInfo receivingPlayer;
    FText text;
    uint8_t pad1[0x4];
    uint8_t pad2[0x1];
    uint8_t pad3[0x10];
};

static_assert(offsetof(FBrickChatMessage, sourcePlayer) == 0x8);
static_assert(offsetof(FBrickChatMessage, receivingPlayer) == 0x40);
static_assert(offsetof(FBrickChatMessage, text) == 0x78);
static_assert(alignof(FBrickChatMessage) == 0x8, "Wrong alignment of FBrickChatMessage");
//static_assert(sizeof(FBrickChatMessage) == 0xa8, "Wrong size of FBrickChatMessage");

struct __attribute((aligned(0x1))) ABrickGameSessionStruct {
    uint8_t data[0x2a8];
};

static_assert(alignof(ABrickGameSessionStruct) == 0x1, "Wrong alignment of ABrickGameSession");
static_assert(sizeof(ABrickGameSessionStruct) == 0x2a8, "Wrong size of ABrickGameSession");

struct FakeABPC {
    uint8_t data[0x227];
    uint64_t value = 0;
};
static_assert(offsetof(FakeABPC, value) == 0x228);
