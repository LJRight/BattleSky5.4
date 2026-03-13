// Fill out your copyright notice in the Description page of Project Settings.


#include "DragIconWidget.h"

void UDragIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	float X, Y;
	GetOwningPlayer()->GetMousePosition(X, Y);

	SetPositionInViewport(FVector2D(X, Y));
}
