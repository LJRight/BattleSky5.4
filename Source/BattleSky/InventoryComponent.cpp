// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "ItemBase.h"
#include "WeaponBase.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// ¹«±â ½½·ÔÀº ÃÑ 4°³
	WeaponSlots.SetNum((int32)EWeaponSlot::MAX);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UInventoryComponent::EquipItem(AItemBase* Item)
{
	if (!Item)
	{
		return false;
	}

	if (AWeaponBase* Weapon = Cast<AWeaponBase>(Item))
	{
		return EquipWeapon(Weapon);
	}
	return false;
}

bool UInventoryComponent::EquipWeapon(AWeaponBase* Weapon)
{
	if (!Weapon) return false;

    switch (Weapon->WeaponType)
    {
    case EWeaponType::Pistol:
        if (!WeaponSlots[(int32)EWeaponSlot::Sidearm])
        {
            WeaponSlots[(int32)EWeaponSlot::Sidearm] = Weapon;
            return true;
        }
        break;

    case EWeaponType::Melee:
        if (!WeaponSlots[(int32)EWeaponSlot::Melee])
        {
            WeaponSlots[(int32)EWeaponSlot::Melee] = Weapon;
            return true;
        }
        break;

    default:
        // AR / SR / DMR / SMG / Shotgun
        if (!WeaponSlots[(int32)EWeaponSlot::Primary])
        {
            WeaponSlots[(int32)EWeaponSlot::Primary] = Weapon;
            return true;
        }
        else if (!WeaponSlots[(int32)EWeaponSlot::Secondary])
        {
            WeaponSlots[(int32)EWeaponSlot::Secondary] = Weapon;
            return true;
        }
        break;
	}
    return false;
}

