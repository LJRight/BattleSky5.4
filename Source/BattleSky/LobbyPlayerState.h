// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public :
	UPROPERTY(ReplicatedUsing = OnRep_LobbyPlayerName)
	FString LobbyPlayerName;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetLobbyPlayerName(const FString& NewName);

	UFUNCTION()
	void OnRep_LobbyPlayerName();
private:
	
	void UpdateUI();
};
