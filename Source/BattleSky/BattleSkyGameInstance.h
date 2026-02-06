// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BattleSkyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API UBattleSkyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;
	virtual void OnStart() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetUIWidget();

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FString PlayerName;
};
