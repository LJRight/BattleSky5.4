// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class ABattleSkyPlayerController;

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class BATTLESKY_API IInteractable
{
	GENERATED_BODY()
public:
	virtual bool TryInteract() = 0;
	virtual AActor* Interact() = 0;
	/*virtual void EnableInteraction(bool bEnable) = 0;*/
	virtual FText GetText() const = 0;
};
