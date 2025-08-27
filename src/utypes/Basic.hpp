// Licensed under MIT, copyright aaron wilk
// https://raw.githubusercontent.com/tubaplayerdis/BRSD/7dc804246fc415dbe629e0c054e39ce84cf05cd0/Include/SDK/Basic.hpp


#include <cstdint>
#include <iostream>
#include "UnrealContainers.hpp"

namespace FTextImpl
{
// Predefined struct FTextData
// 0x0038 (0x0038 - 0x0000)
class FTextData final
{
public:
	uint8_t                                         Pad_0[0x28];                                       // 0x0000(0x0028)(Fixing Size After Last Property [ Dumper-7 ])
	class UC::FString                                   TextSource;                                        // 0x0028(0x0010)(NOT AUTO-GENERATED PROPERTY)
};
static_assert(alignof(FTextData) == 0x000008, "Wrong alignment on FTextData");
static_assert(sizeof(FTextData) == 0x000038, "Wrong size on FTextData");
static_assert(offsetof(FTextData, TextSource) == 0x000028, "Member 'FTextData::TextSource' has a wrong offset!");
}

// Predefined struct FText
// 0x0018 (0x0018 - 0x0000)
class FText final
{
public:
	class FTextImpl::FTextData*                   TextData;                                          // 0x0000(0x0008)(NOT AUTO-GENERATED PROPERTY)
	uint8_t                                       Pad_8[0x10];                                       // 0x0008(0x0010)(Fixing Struct Size After Last Property [ Dumper-7 ])

public:
	const class UC::FString& GetStringRef() const
	{
		return TextData->TextSource;
	}
	std::string ToString() const
	{
		return TextData->TextSource.ToString();
	}
};
static_assert(alignof(FText) == 0x000008, "Wrong alignment on FText");
static_assert(sizeof(FText) == 0x000018, "Wrong size on FText");
static_assert(offsetof(FText, TextData) == 0x000000, "Member 'FText::TextData' has a wrong offset!");

// Predefined struct FWeakObjectPtr
// 0x0008 (0x0008 - 0x0000)
class FWeakObjectPtr
{
public:
	int32_t                                         ObjectIndex;                                       // 0x0000(0x0004)(NOT AUTO-GENERATED PROPERTY)
	int32_t                                         ObjectSerialNumber;                                // 0x0004(0x0004)(NOT AUTO-GENERATED PROPERTY)_

public:
	class UObject* Get() const;
	class UObject* operator->() const;
	bool operator==(const FWeakObjectPtr& Other) const;
	bool operator!=(const FWeakObjectPtr& Other) const;
	bool operator==(const class UObject* Other) const;
	bool operator!=(const class UObject* Other) const;
};
static_assert(alignof(FWeakObjectPtr) == 0x000004, "Wrong alignment on FWeakObjectPtr");
static_assert(sizeof(FWeakObjectPtr) == 0x000008, "Wrong size on FWeakObjectPtr");
static_assert(offsetof(FWeakObjectPtr, ObjectIndex) == 0x000000, "Member 'FWeakObjectPtr::ObjectIndex' has a wrong offset!");
static_assert(offsetof(FWeakObjectPtr, ObjectSerialNumber) == 0x000004, "Member 'FWeakObjectPtr::ObjectSerialNumber' has a wrong offset!");

template<typename UEType>
class TWeakObjectPtr : public FWeakObjectPtr
{
public:
	UEType* Get() const
	{
		return static_cast<UEType*>(FWeakObjectPtr::Get());
	}

	UEType* operator->() const
	{
		return static_cast<UEType*>(FWeakObjectPtr::Get());
	}
};
