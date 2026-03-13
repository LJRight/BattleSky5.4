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

bool UInventoryComponent::EquipItem(AItemBase* Item, int& OutSlotIndex)
{
	if (!Item)
	{
		return false;
	}

	if (AWeaponBase* Weapon = Cast<AWeaponBase>(Item))
	{
		return EquipWeapon(Weapon, OutSlotIndex);
	}
	return false;
}

bool UInventoryComponent::EquipWeapon(AWeaponBase* Weapon, int& OutSlotIndex)
{
	if (!Weapon) return false;

    switch (Weapon->WeaponType)
    {
    case EWeaponType::Pistol:
        if (!WeaponSlots[(int32)EWeaponSlot::Sidearm])
        {
            WeaponSlots[(int32)EWeaponSlot::Sidearm] = Weapon;
            OutSlotIndex = (int32)EWeaponSlot::Sidearm;
            return true;
        }
        break;

    case EWeaponType::Melee:
        if (!WeaponSlots[(int32)EWeaponSlot::Melee])
        {
            WeaponSlots[(int32)EWeaponSlot::Melee] = Weapon;
            OutSlotIndex = (int32)EWeaponSlot::Melee;
            return true;
        }
        break;

    default:
        // AR / SR / DMR / SMG / Shotgun
        if (!WeaponSlots[(int32)EWeaponSlot::Primary])
        {
            WeaponSlots[(int32)EWeaponSlot::Primary] = Weapon;
            OutSlotIndex = (int32)EWeaponSlot::Primary;
            return true;
        }
        else if (!WeaponSlots[(int32)EWeaponSlot::Secondary])
        {
            WeaponSlots[(int32)EWeaponSlot::Secondary] = Weapon;
			OutSlotIndex = (int32)EWeaponSlot::Secondary;
            return true;
        }
        break;
	}
    return false;
}

