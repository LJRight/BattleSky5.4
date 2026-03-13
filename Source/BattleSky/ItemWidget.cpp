// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UIManagerSubsystem.h"
#include "ItemBase.h"

void UItemWidget::Setup(AItemBase* ItemInfo)
{
	if (ItemInfo->Icon)
	{
		this->Icon->SetBrushFromTexture(ItemInfo->Icon);
	}
	ItemName->SetText(ItemInfo->GetText());
}

void UItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
		UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
		if (UIManager->DragManager)
		{
			UIManager->DragManager->StartDrag(this);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No UI Manager"));
		}
		return FReply::Handled();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can Ge Event"));
	}
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (UIManager->DragManager)
	{
		UIManager->DragManager->EndDrag();
	}

	return FReply::Handled();
}
