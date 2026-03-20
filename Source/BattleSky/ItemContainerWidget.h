// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "ItemBase.h"
#include "SlotWidget.h"
#include "ItemContainerWidget.generated.h"

class UItemWidget;

UCLASS()
class BATTLESKY_API UItemContainerWidget : public UUserWidget, public ISlotWidget
{
	GENERATED_BODY()

public:
	void UpdateList(const TArray<class AItemBase*>& List);

	virtual bool CanAcceptItem(UItemWidget* ItemWidget) const;
	virtual void OnItemDropped(UItemWidget* ItemWidget);


private :
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<EItemType> AcceptableItemType;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Container;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf< UItemWidget> ItemWidgetClass;
};
