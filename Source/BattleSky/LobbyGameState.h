// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	TArray<FString> PlayerNames;

	void AddPlayerName(const FString& NewPlayerName);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_UpdatePlayerNames(const TArray<FString>& UpdatedPlayerNames);
	void MultiCast_UpdatePlayerNames_Implementation(const TArray<FString>& UpdatedPlayerNames);
};
