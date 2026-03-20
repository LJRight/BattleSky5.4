// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "ItemBase.h"
#include "WeaponBase.h"
#include "BattleSkyCharacter.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// 무기 슬롯은 총 4개
	WeaponSlots.SetNum((int32)EWeaponSlot::MAX);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningCharacter = Cast<ABattleSkyCharacter>(GetOwner());
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

// 아이템의 상호작용에서 호출됨
void UInventoryComponent::PickUpItem(AItemBase* TargetItem)
{
    if (CanPickUp(TargetItem->Weight))
	{
		TargetItem->EnableInteraction(false);
        if (TargetItem->Type == EItemType::Weapon || TargetItem->Type == EItemType::Armor)
        {
            
            EquipItem(TargetItem);
			return;
        }
    }
    else
    {

    }
	return;
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

bool UInventoryComponent::EquipWeapon(AWeaponBase* TargetWeapon)
{
	if (!TargetWeapon)
	{
		return false;
	}

    
    switch (TargetWeapon->WeaponType)
    {
    case EWeaponType::Pistol:
        if (!WeaponSlots[(int32)EWeaponSlot::Sidearm])
        {
            WeaponSlots[(int32)EWeaponSlot::Sidearm] = TargetWeapon;
			// OutSlotIndex = (int32)EWeaponSlot::Sidearm;
            return true;
        }
        break;
    
    case EWeaponType::Melee:
        if (!WeaponSlots[(int32)EWeaponSlot::Melee])
        {
            WeaponSlots[(int32)EWeaponSlot::Melee] = TargetWeapon;
            //OutSlotIndex = (int32)EWeaponSlot::Melee;
            return true;
        }
        break;

    default:
        // AR / SR / DMR / SMG / Shotgun

        // 주요 로직. 주무기 칸 확인 -> 비어있으면 해당 슬롯에 장착, 
        // 비어있지 않다면 부무기 확인 -> 비어있다면 해당 슬롯에 장착
        // 부무기까지 장착되어 있으면, 현재 주무기와 교체

        AWeaponBase* PrimarySlotWeapon = GetWeapon((int32)EWeaponSlot::Primary);
		AWeaponBase* SecondarySlotWeapon = GetWeapon((int32)EWeaponSlot::Secondary);
		AWeaponBase* CurrentWeapon = OwningCharacter->Replicated_CurrentEquipedWeapon;

		bool bHasPrimary = PrimarySlotWeapon != nullptr;
		bool bHasSecondary = SecondarySlotWeapon != nullptr;
		bool bIsHoldingWeapon = CurrentWeapon != nullptr;

        // 첫 번째 슬롯이 비어있을 때
        if (!bHasPrimary)
        {
		    // 슬롯에 넣기
			WeaponSlots[(int32)EWeaponSlot::Primary] = TargetWeapon;
            // 무기를 들고 있다면
            if (bIsHoldingWeapon)
            {
                // 몸에 붙이기
				OwningCharacter->AttachToBody(TargetWeapon, EWeaponSlot::Primary);
			}
            // 안 들고 있다면
            else
            {
                // 손으로 들기
                OwningCharacter->HoldWeapon(TargetWeapon);
            }
			return true;
        }
        // 첫 번째 슬롯은 있고, 두 번째 슬롯이 비어있을 때
        if (!bHasSecondary)
        {
            // 슬롯에 넣고
			WeaponSlots[(int32)EWeaponSlot::Secondary] = TargetWeapon;
            // 무기를 들고 있든 아니든
			// 몸에 붙이기
			OwningCharacter->AttachToBody(TargetWeapon, EWeaponSlot::Secondary);
			return true;
        }
        // 두 슬롯 모두 있을 때,
        else
        {
			OwningCharacter->DropItem(SecondarySlotWeapon);
            WeaponSlots[(int32)EWeaponSlot::Secondary] = TargetWeapon;
            OwningCharacter->AttachToBody(TargetWeapon, EWeaponSlot::Secondary);
			return true;
    //        // 무기를 들고 있다면,
    //        if (bIsHoldingWeapon)
    //        {
    //            // 들고 있는 것을 바닥에 내려놓고, 무기 장착
				//OwningCharacter->DropItem(CurrentWeapon);
				//OwningCharacter->HoldWeapon(TargetWeapon);
    //        }
    //        // 무기를 안 들고 있으면,
    //        else
    //        {
    //            // 부무장을 바닥에 버리고, 등에 부착
				//OwningCharacter->DropItem(SecondarySlotWeapon);
				//WeaponSlots[(int32)EWeaponSlot::Secondary] = TargetWeapon;
				//OwningCharacter->AttachToBody(TargetWeapon, EWeaponSlot::Secondary);
    //        }
        }
        

		/*if (AWeaponBase* CurrentPrimary = GetWeapon((int32)EWeaponSlot::Primary))
        {
            if (AWeaponBase* CurrentSecondary = GetWeapon((int32)EWeaponSlot::Secondary))
            {
                if (OwningCharacter)
                {
					OwningCharacter->DropItem(CurrentPrimary);
					WeaponSlots[(int32)EWeaponSlot::Primary] = TargetWeapon;
                    OwningCharacter->PickupItem(TargetWeapon, );
                }
            }
            WeaponSlots[(int32)EWeaponSlot::Primary] = Weapon;
            OutSlotIndex = (int32)EWeaponSlot::Primary;
            return true;
        }
        else if (!WeaponSlots[])
        {
            WeaponSlots[(int32)EWeaponSlot::Secondary] = Weapon;
			OutSlotIndex = (int32)EWeaponSlot::Secondary;
            return true;
        }*/
        break;
	}
    return false;
}





