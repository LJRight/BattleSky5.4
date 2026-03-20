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
	
	void UpdateInventoryList(const TArray<class AItemBase*>& List);
	void UpdateNearbyItemsList(const TArray<AItemBase*>& List);
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	class UItemContainerWidget* InventoryItemsList;
	UPROPERTY(meta = (BindWidget))
	UItemContainerWidget* NearByItemsList;
};
