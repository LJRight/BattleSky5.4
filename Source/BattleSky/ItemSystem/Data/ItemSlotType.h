#pragma once

#include "CoreMinimal.h"
#include "ItemSlotType.generated.h"

UENUM(BlueprintType)
enum class ESlotGroup : uint8
{
	World UMETA(DisplayName = "World"),
	Inventory UMETA(DisplayName = "Inventory"),
	Equipment UMETA(DisplayName = "Equipment"),
	Weapon UMETA(DisplayName = "Weapon"),
	Attachment UMETA(DisplayName = "Attachment"),
	None,
};

UENUM(BlueprintType)
enum class EWeaponSlotType : uint8
{
	Primary UMETA(DisplayName = "Primary") ,
	Secondary UMETA(DisplayName = "Secondary"),
	Sidearm UMETA(DisplayName = "Sidearm"),
	Melee UMETA(DisplayName = "Melee"),
	Throwable UMETA(DisplayName = "Throwable"),
	None,
};

UENUM(BlueprintType)
enum class EAttachmentSlotType : uint8
{
	Muzzle UMETA(DisplayName = "Muzzle"),
	Grip UMETA(DisplayName = "Grip"),
	Magazine UMETA(DisplayName = "Magazine"),
	Scope UMETA(DisplayName = "Scope"),
	None
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	Backpack UMETA(DisplayName = "Backpack"), 
	Vest UMETA(DisplayName = "Vest"),
	Helmet UMETA(DisplayName = "Helmet"),
	None
};

UENUM(BlueprintType)
enum class ESlotType : uint8
{
	NearBy UMETA(DisplayName = "NearBy"),
	Inventory UMETA(DisplayName = "Inventory"),
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Sidearm UMETA(DisplayName = "Sidearm"),
	Melee UMETA(DisplayName = "Melee"),
	Throwable UMETA(DisplayName = "Throwable"),
	Muzzle UMETA(DisplayName = "Muzzle"),
	Grip UMETA(DisplayName = "Grip"),
	Magazine UMETA(DisplayName = "Magazine"),
	Scope UMETA(DisplayName = "Scope"),
	Backpack UMETA(DisplayName = "Backpack"),
	Vest UMETA(DisplayName = "Vest"),
	Helmet UMETA(DisplayName = "Helmet"),
};

USTRUCT(BlueprintType)
struct FSlotInfo
{
	GENERATED_BODY()
public:
	FSlotInfo(ESlotGroup Group = ESlotGroup::None, EWeaponSlotType WeaponSlot = EWeaponSlotType::None,
		EAttachmentSlotType AttachmentSlot = EAttachmentSlotType::None, EEquipmentSlot EquipmentSlot = EEquipmentSlot::None)
		: Group(Group), WeaponSlot(WeaponSlot), AttachmentSlot(AttachmentSlot), EquipmentSlot(EquipmentSlot)
	{
	};
	ESlotGroup Group;
	EWeaponSlotType WeaponSlot;
	EAttachmentSlotType AttachmentSlot;
	EEquipmentSlot EquipmentSlot;
};