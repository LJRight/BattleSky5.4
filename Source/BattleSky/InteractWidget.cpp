// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractWidget.h"
#include "Components/TextBlock.h"

void UInteractWidget::SetText(const FText& NewText)
{
	if (InteractText)
	{
		InteractText->SetText(NewText);
	}
}