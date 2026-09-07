// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "ItemSystem/Actor/ItemActor.h"

#include "BattleSkyCharacter.h"
#include "BattleSkyPlayerController.h"
#include "InventoryComponent.h"

#include "Components/ProgressBar.h"
#include "../Source/BattleSky/ItemSystem/UI/MultiSlotContainerWidget.h"
#include "../Source/BattleSky/ItemSystem/UI/SingleSlotContainerWidget.h"

void UInventoryWidget::Init()
{
	if (ABattleSkyCharacter* BSCharacter = Cast<ABattleSkyCharacter>(GetOwningPlayer()->GetCharacter()))
	{
		BSCharacter->OnNearByUpdated.AddUObject(this, &UInventoryWidget::NearByUpdate);
	}
	if (ABattleSkyPlayerController* Controller = Cast<ABattleSkyPlayerController>(GetOwningPlayer()))
	{
		if (Controller->Inventory)
		{
			Controller->Inventory->OnInventoryUpdated.AddUObject(this, &UInventoryWidget::InventoryUpdate);
			// Controller->Inventory->OnWeaponSlotUpdated.AddUObject(this, &UInventoryWidget::InventoryUpdate)
		}
	}
	NearByList->SetOwningPlayer(GetOwningPlayer());
	InventoryList->SetOwningPlayer(GetOwningPlayer());
	WeaponSlots.SetNum(5);
	WeaponSlots[0] = PrimaryWeaponSlot;
	WeaponSlots[1] = SecondaryWeaponSlot;
	WeaponSlots[2] = SidearmWeaponSlot;
	WeaponSlots[3] = MeleeWeaponSlot;
	WeaponSlots[4] = ThrowableWeaponSlot;
}

void UInventoryWidget::NearByUpdate(const TArray<AItemActor*>& WorldItemActors)
{
	TArray<FItemViewData> Datas;
	for (AItemActor* Item : WorldItemActors)
	{
		Datas.Add(FItemViewData(FGuid(), Item->ItemData, Item));
	}
	NearByList->Update(Datas);
}

void UInventoryWidget::InventoryUpdate(const TArray<FInventoryItemData>& InventoryItemDatas, float WeightRatio)
{
	TArray<FItemViewData> Datas;
	for (const FInventoryItemData& ItemData : InventoryItemDatas)
	{
		Datas.Add(FItemViewData(ItemData.ID, ItemData.Data));
	}
	InventoryList->Update(Datas);
	InventorySpace->SetPercent(WeightRatio);
}
