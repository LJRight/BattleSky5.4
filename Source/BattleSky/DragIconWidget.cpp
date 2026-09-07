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