// Fill out your copyright notice in the Description page of Project Settings.


#include "DragIconWidget.h"
#include "Components/Image.h"

void UDragIconWidget::SetIcon(UImage* Icon)
{
    if (Icon)
	{        
        this->MyIcon = Icon;
	}
}

void UDragIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!GetOwningPlayer())
        return;

    float X, Y;

    if (GetOwningPlayer()->GetMousePosition(X, Y))
    {
        SetPositionInViewport(FVector2D(X, Y));
    }
}