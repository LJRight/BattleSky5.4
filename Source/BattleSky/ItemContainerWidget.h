// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemBase.h"
#include "ItemContainerWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API UItemContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateList(const TArray<class AItemBase*>& List);

private :
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<EItemType> AcceptableItemType;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Container;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UItemWidget> ItemWidgetClass;
};
