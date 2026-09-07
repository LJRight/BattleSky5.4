#include "LobbyPlayerController.h"
#include "UIManagerSubsystem.h"
#include "LobbyGameState.h"
#include "LobbyPlayerState.h"
#include "BattleSkyGameInstance.h"
#include "LobbyGameMode.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	UE_LOG(LogTemp, Warning, TEXT("LobbyPlayerController CONSTRUCTED"));
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Lobby Player Controller Show Main Menu"));
			UI->ShowSessionLobby(this, HasAuthority());
		}
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
		
		if (UBattleSkyGameInstance* BSGameInstance = Cast<UBattleSkyGameInstance>(GetGameInstance()))
		{
			Server_SendLobbyPlayerName(BSGameInstance->PlayerName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Find GameInstance in LobbyPlayerController"));
		}
	}
}

void ALobbyPlayerController::Server_SendLobbyPlayerName_Implementation(const FString& PlayerName)
{
	if (ALobbyPlayerState*  LobbyPlayerState = GetPlayerState<ALobbyPlayerState>())
	{
		LobbyPlayerState->SetLobbyPlayerName(PlayerName);
	}
}

void ALobbyPlayerController::ShowLobbyUI()
{
	if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Lobby Player Controller Show Main Menu"));
		UI->ShowSessionLobby(this, HasAuthority());
	}
}

void ALobbyPlayerController::Server_RequestStartGame_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}
	if (ALobbyGameMode* GM = Cast< ALobbyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->StartGame();
	}
}