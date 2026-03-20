// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "UIManagerSubsystem.h"
#include "SlotWidget.h"
#include "ItemBase.h"

void UItemWidget::Setup(AItemBase* ItemInfo, ESlotType SpawnerSlotType)
{
	if (ItemInfo)
	{
		ItemActor = ItemInfo;
		OwnerSlotType = SpawnerSlotType;
		if (ItemInfo->Icon)
		{
			this->Icon->SetBrushFromTexture(ItemInfo->Icon);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Icon"));
		}

	}
	ItemName->SetText(ItemInfo->GetText());
}

// 아이템 위젯에 호버될 시 마우스
void UItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (Background)
	{
		Background->SetBrushColor(HoverColor);
	}
}

void UItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (Background)
	{
		Background->SetBrushColor(BaseColor);
	}
}

// 아이템 위젯에서 드래그 시작
FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
		UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
		if (UI && UI->DragManager)
		{
			UI->DragManager->StartDrag(this);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No UI Manager"));
		}
		return FReply::Handled();
	}
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

// 드래그 종료 시
FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
		if (UI && UI->DragManager && UI->DragManager->IsDragging())
		{
			UI->DragManager->EndDrag();
		}
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}
