// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"

#include "ItemWidget.h"
#include "ItemBase.h"

void UInventoryWidget::UpdateNearbyItemsList(const TArray<AItemBase*> List)
{
    if (!NearbyItems)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find NearByItems Vertical Box"));
        return;
    }
    NearbyItems->ClearChildren();

	for (const auto& Item : List)
    {
		UItemWidget* ItemWidget = CreateWidget<UItemWidget>(GetWorld(), ItemWidgetClass);
        if (ItemWidget)
        {
			ItemWidget->Setup(Item);
            if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(NearbyItems->AddChild(ItemWidget)))
            {
				ScrollBoxSlot->SetPadding(FMargin(0.f, 5.f));
			}
        }
    }
}
