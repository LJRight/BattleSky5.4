// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemBase.h"
#include "DropSlot.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDropSlot : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLESKY_API IDropSlot
{
	GENERATED_BODY()

public:
	virtual void CanAcceptItem(EItemType Type) const = 0;
	virtual void OnItemDropped() = 0;
	virtual void SetItem() = 0;
};
