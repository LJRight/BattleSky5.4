#include "LobbyPlayerController.h"
#include "UIManagerSubsystem.h"
#include "LobbyGameState.h"
#include "BattleSkyGameInstance.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	UE_LOG(LogTemp, Warning, TEXT("LobbyPlayerController CONSTRUCTED"));
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{

		/*GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ALobbyPlayerController::ShowLobbyUI);*/
		if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Lobby Player Controller Show Main Menu"));
			UI->ShowSessionLobby(this);
		}
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;

	}
	Server_SendPlayerNameToGameState(Cast<UBattleSkyGameInstance>(GetGameInstance())->PlayerName);
}

void ALobbyPlayerController::Server_SendPlayerNameToGameState_Implementation(const FString& PlayerName)
{
	if (ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>())
	{
		GS->AddPlayerName(PlayerName);
	}
}

void ALobbyPlayerController::ShowLobbyUI()
{
	if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Lobby Player Controller Show Main Menu"));
		UI->ShowSessionLobby(this);
	}
}