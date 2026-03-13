// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ItemBase.h"

void UItemWidget::Setup(AItemBase* ItemInfo)
{
	if (ItemInfo->Icon)
	{
		this->Icon->SetBrushFromTexture(ItemInfo->Icon);
	}
	ItemName->SetText(ItemInfo->GetText());
}
