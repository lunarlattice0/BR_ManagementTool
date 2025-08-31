// A trimmed down reimplementation of UnrealContainers.
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
        inline std::wstring ToWString() const {
            if (*this)
                return std::wstring(Data);

            return L"";
        }
        //inline wchar_t* CStr() {return Data;}
        //inline const wchar_t* CStr() const {return Data;}

};
