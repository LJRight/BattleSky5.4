// Fill out your copyright notice in the Description page of Project Settings.


#include "DragManager.h"
#include "ItemWidget.h"

// 인벤토리에서 드래그 시작 시 호출
void UDragManager::StartDrag(UItemWidget* InWidget)
{
    if (!InWidget)
    {
        return;
    }

    DraggingWidget = InWidget;
    bDragging = true;

    if (DragIconWidgetClass)
    {
		DragIconWidget = CreateWidget<UDragIconWidget>(InWidget->GetOwningPlayer(), DragIconWidgetClass);

        if (DragIconWidget)
        {
            DragIconWidget->SetIcon(InWidget->Icon);

			// 마우스 이벤트 무시 (아래 위젯이 입력 받도록)
            DragIconWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
            DragIconWidget->AddToViewport(9999);
		}
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Drag Icon Widget Class"));
    }
}

void UDragManager::EndDrag()
{
    bDragging = false;
    
	DraggingWidget = nullptr;

    if (DragIconWidget)
    {
        DragIconWidget->RemoveFromParent();
        DragIconWidget = nullptr;
    }
}

bool UDragManager::IsDragging() const
{
    return bDragging;
}

UItemWidget* UDragManager::GetDraggingWidget() const
{
    return DraggingWidget;
}