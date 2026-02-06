// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"
#include "Net/UnrealNetwork.h"
#include "UIManagerSubsystem.h"


void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ALobbyGameState, PlayerNames);
}

void ALobbyGameState::AddPlayerName(const FString& NewName)
{
    if (!HasAuthority()) return;

    if (!PlayerNames.Contains(NewName))
    {
        PlayerNames.Add(NewName);
        MultiCast_UpdatePlayerNames(PlayerNames);
    }
}

// 멀티캐스트 RPC 구현
void ALobbyGameState::MultiCast_UpdatePlayerNames_Implementation(const TArray<FString>& UpdatedNames)
{
    if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
    {
        UIManager->UpdateLobbyPlayerNames(UpdatedNames);
    }
}