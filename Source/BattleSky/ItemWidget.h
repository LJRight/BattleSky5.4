// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(class AItemBase* ItemInfo);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
	class UImage* Icon;
};
