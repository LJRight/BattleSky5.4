// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "UIManagerSubsystem.h"

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyPlayerState, LobbyPlayerName);
}

void ALobbyPlayerState::SetLobbyPlayerName(const FString& NewName)
{
	LobbyPlayerName = NewName;
	UpdateUI();
}

void ALobbyPlayerState::OnRep_LobbyPlayerName()
{
	UpdateUI();
}

void ALobbyPlayerState::UpdateUI()
{
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->UpdateLobbyPlayerNames();
	}
}