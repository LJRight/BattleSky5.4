// Fill out your copyright notice in the Description page of Project Settings.


#include "DragManager.h"
#include "ItemWidget.h"

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
            DragIconWidget->AddToViewport(9999);
		}
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No DragIcon Widget Class"));
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