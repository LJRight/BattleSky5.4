// Fill out your copyright notice in the Description page of Project Settings.


//#include "ItemContainerWidget.h"
//#include "Components/ScrollBox.h"
//#include "Components/ScrollBoxSlot.h"
//#include "ItemSystem/Actor/ItemActor.h"
//#include "ItemWidget.h"
//#include "UIManagerSubsystem.h"


//FReply UItemContainerWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
//	{
//		if (UI->DragManager && UI->DragManager->IsDragging())
//		{
//			UItemWidget* TargetItemWidget = UI->DragManager->GetDraggingItemWidget();
//			if (CanAcceptItem(TargetItemWidget)
//			{
//				OnItemDropped(TargetItemWidget);
//			}
//			UI->DragManager->EndDrag();
//		}
//		return FReply::Handled();
//	}
//	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
//}

//void UItemContainerWidget::UpdateList(const TArray<AItemBase*>& List)
//{
//	if (!Container)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Can't Find NearByItems Vertical Box"));
//		return;
//	}
//	Container->ClearChildren();
//
//	if (ItemWidgetClass)
//	{
//		for (const auto& Item : List)
//		{
//			UItemWidget* ItemWidget = CreateWidget<UItemWidget>(GetWorld(), ItemWidgetClass);
//			if (ItemWidget)
//			{
//				ItemWidget->Setup(Item, SlotType);
//				if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(Container->AddChild(ItemWidget)))
//				{
//					ScrollBoxSlot->SetPadding(FMargin(0.f, 5.f));
//				}
//			}
//		}
//	}
//}
//
//bool UItemContainerWidget::CanAcceptItem(UItemWidget* ItemWidget) const
//{
//	return AcceptableItemType.Contains(ItemWidget->GetItem()->Type) && SlotType != ItemWidget->GetOwnerSlotType();
//}
//
//void UItemContainerWidget::OnItemDropped(UItemWidget* ItemWidget)
//{
//	ESlotType SourceSlot = ItemWidget->GetOwnerSlotType();
//	SlotType;
//}
