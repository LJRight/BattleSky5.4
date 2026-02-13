// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"
#include "Net/UnrealNetwork.h"
#include "UIManagerSubsystem.h"


void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ALobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);	
	OnLobbyPlayerListChanged.Broadcast();
}

void ALobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	OnLobbyPlayerListChanged.Broadcast();
}
