// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "LobbyGameState.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnLobbyPlayerListChanged);


UCLASS()
class BATTLESKY_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnLobbyPlayerListChanged OnLobbyPlayerListChanged;

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	void NotifyPlayerListChanged() { OnLobbyPlayerListChanged.Broadcast(); }
};
