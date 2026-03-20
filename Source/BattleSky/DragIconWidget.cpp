// Fill out your copyright notice in the Description page of Project Settings.


#include "DragIconWidget.h"
#include "Components/Image.h"

void UDragIconWidget::SetIcon(UImage* Icon)
{
    if (Icon && MyIcon)
	{        
		this->MyIcon->SetBrush(Icon->GetBrush());
	}
    else
    {
		UE_LOG(LogTemp, Warning, TEXT("No Icon in Item"));
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
		SetPositionInViewport(FVector2D(X - WidgetSize.X * 0.5f, Y - WidgetSize.Y * 0.5f));
    }
}