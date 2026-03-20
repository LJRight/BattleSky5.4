// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SlotWidget.generated.h"

UENUM(BlueprintType)
enum class ESlotType : uint8
{
	NearBy UMETA(DisplayName = "NearBy"),
	Inventory UMETA(DisplayName = "Inventory"), 
	Equip UMETA(DisplayName = "Equip"),
	Parts UMETA(DisplayName = "Parts"),
};


UINTERFACE(MinimalAPI)
class USlotWidget : public UInterface
{
	GENERATED_BODY()
};

class BATTLESKY_API ISlotWidget
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	ESlotType SlotType;
	virtual bool CanAcceptItem(class UItemWidget* ItemWidget) const = 0;
	virtual void OnItemDropped(class UItemWidget* ItemWidget) = 0;
};
