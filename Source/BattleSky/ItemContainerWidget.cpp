// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemContainerWidget.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "ItemBase.h"
#include "ItemWidget.h"
#include "UIManagerSubsystem.h"

void UItemContainerWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		if (UI->DragManager)
		{
			UI->DragManager->SetCurrentSlotType(AcceptableItemType);
		}
	}
}

void UItemContainerWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		if (UI->DragManager)
		{
			UI->DragManager->ClearCurrentSlotType();
		}
	}
}

FReply UItemContainerWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
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

void UItemContainerWidget::UpdateList(const TArray<AItemBase*>& List)
{
	if (!Container)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find NearByItems Vertical Box"));
		return;
	}
	Container->ClearChildren();

	if (ItemWidgetClass)
	{
		for (const auto& Item : List)
		{
			UItemWidget* ItemWidget = CreateWidget<UItemWidget>(GetWorld(), ItemWidgetClass);
			if (ItemWidget)
			{
				ItemWidget->Setup(Item);
				if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(Container->AddChild(ItemWidget)))
				{
					ScrollBoxSlot->SetPadding(FMargin(0.f, 5.f));
				}
			}
		}
	}
}
