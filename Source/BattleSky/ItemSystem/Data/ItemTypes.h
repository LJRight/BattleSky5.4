#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Consumable UMETA(DisplayName = "Consumable"),
	Attachment UMETA(DisplayName = "Attachment"),
	Ammo UMETA(DisplayName = "Ammo"),
	Equipment UMETA(DisplayName = "Equipment"),
	Throwable UMETA(DisplayName = "Throwable"),
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Ammo556 UMETA(DisplayName = "5.56mm"),
	Ammo762 UMETA(DisplayName = "7.62mm"),
};

UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
	Muzzle UMETA(DisplayName = "Muzzle"),
	Grip UMETA(DisplayName = "Grip"),
	Magazine UMETA(DisplayName = "Magazine"),
	Scope UMETA(DisplayName = "Scope"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AR UMETA(DisplayName = "AR"),
	SR UMETA(DisplayName = "SR"),
	DMR UMETA(DisplayName = "DMR"),
	ShotGun UMETA(DisplayName = "ShotGun"),
	Pistol UMETA(DisplayName = "Pistol"),
	Melee UMETA(DisplayName = "Melee"),
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SemiAuto UMETA(DisplayName = "SemiAuto"),
	FullAuto UMETA(DisplayName = "FullAuto"),
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Sidearm UMETA(DisplayName = "Sidearm"),
	Melee UMETA(DisplayName = "Melee"),
	Throwable UMETA(DisplayName = "Throwable"),
	MAX,
};

UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	Helmet UMETA(DisplayName = "Helmet"),
	Vest UMETA(DisplayName = "Vest"),
	Backpack UMETA(DisplayName = "Backpack"),
	MAX,
};
