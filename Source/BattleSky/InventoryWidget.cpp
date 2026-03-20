// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "UIManagerSubsystem.h"
#include "ItemContainerWidget.h"
#include "ItemBase.h"


void UInventoryWidget::UpdateInventoryList(const TArray<AItemBase*>& List)
{
	if (InventoryItemsList)
	{
		InventoryItemsList->UpdateList(List);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Inventory Items List Widget in Inventory Widget"));
	}
}

void UInventoryWidget::UpdateNearbyItemsList(const TArray<AItemBase*>& List)
{
	if (NearByItemsList)
	{
		NearByItemsList->UpdateList(List);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Near By Items List Widget in Inventory Widget"));
	}
}

FReply UInventoryWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Cancel Drag"));
	if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		if (UI->DragManager)
		{
			UI->DragManager->EndDrag();
		}
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}


