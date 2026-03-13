// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateNearbyItemsList(const TArray<class AItemBase*> List);

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* NearbyItems;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UItemWidget> ItemWidgetClass;
};
