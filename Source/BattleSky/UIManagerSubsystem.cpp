#include "UIManagerSubsystem.h"
#include "LANSessionSubsystem.h"
#include "SessionsListWidget.h"
#include "Blueprint/UserWidget.h"
#include "BattleSkyGameInstance.h"
#include "SessionLobbyWidget.h"

void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UUIManagerSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UUIManagerSubsystem::Subscribe()
{
    if (ULANSessionSubsystem* LANSession = GetGameInstance()->GetSubsystem<ULANSessionSubsystem>())
    {
        UE_LOG(LogTemp, Warning, TEXT("UI Manager Subsystem has completed Event subscribe"));
        LANSession->OnSessionsFound.AddUObject(this, &UUIManagerSubsystem::SetSessionList);
    }
}

void UUIManagerSubsystem::OnConfirmButtonClicked(const FString& PlayerName)
{
    if(UBattleSkyGameInstance* GI =  Cast<UBattleSkyGameInstance>(GetGameInstance()))
    {
        GI->PlayerName = PlayerName;
	}
	OnSessionLogicRequested.Broadcast(bIsCreatingSession);
}

void UUIManagerSubsystem::ShowMainMenu(APlayerController* Owner)
{
	if (!MainMenuWidgetClass || !Owner)
    {
		UE_LOG(LogTemp, Warning, TEXT("UI Manager Can't Create Main Menu Widget"));
        return;
	}
    CurrentWidget = CreateWidget<UUserWidget>(Owner, MainMenuWidgetClass);
    if (CurrentWidget)
	{
        CurrentWidget->AddToViewport();
    }
}

void UUIManagerSubsystem::ShowSessionLobby(APlayerController* Owner, bool bIsHost)
{
    if (!SessionLobbyWidgetClass || !Owner)
    {
        UE_LOG(LogTemp, Warning, TEXT("UI Manager Can't Create Main Menu Widget"));
		return;
    }
    CurrentWidget = CreateWidget<UUserWidget>(Owner, SessionLobbyWidgetClass);
    if (USessionLobbyWidget* LobbyWidget = Cast<USessionLobbyWidget>(CurrentWidget))
    {
        LobbyWidget->GameStartButtonDisplaySetting(bIsHost);
    }
    if (CurrentWidget)
    {
        CurrentWidget->AddToViewport();
    }
}

void UUIManagerSubsystem::StartCreateFlow()
{
    bIsCreatingSession = true;
    SwitchWidget(NameInputWidgetClass);
}

void UUIManagerSubsystem::StartFindFlow()
{
    bIsCreatingSession = false;
    SwitchWidget(NameInputWidgetClass);
}

void UUIManagerSubsystem::BackToMainMenu()
{
    SwitchWidget(MainMenuWidgetClass);
}

void UUIManagerSubsystem::SetSessionList(const TArray<FOnlineSessionSearchResult>& SessionResults)
{
    if (!CurrentWidget || !CurrentWidget->IsA<USessionsListWidget>())
    {
        SwitchWidget(SessionListWidgetClass);
	}

    if (USessionsListWidget* SessionListWidget = Cast<USessionsListWidget>(CurrentWidget))
    {
        SessionListWidget->UpdateSessionList(SessionResults);
    }
}

void UUIManagerSubsystem::UpdateLobbyPlayerNames(const TArray<FString>& PlayerNames)
{
    if(!CurrentWidget || !CurrentWidget->IsA<USessionLobbyWidget>())
    {
		UE_LOG(LogTemp, Warning, TEXT("Current Widget is not SessionLobbyWidget"));
        return;
    }
    if (USessionLobbyWidget* SessionLobbyWidget = Cast<USessionLobbyWidget>(CurrentWidget))
    {
        SessionLobbyWidget->UpdateLobbyPlayerNames(PlayerNames);
	}
}

void UUIManagerSubsystem::SwitchWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    if (!NewWidgetClass)
    {
        return;
    }
    if (APlayerController* PlayerController = GetOwnerController())
    {
        CurrentWidget = CreateWidget<UUserWidget>(PlayerController, NewWidgetClass);
        if (CurrentWidget)
        {
            CurrentWidget->AddToViewport();
		}

		// UI 입력 모드
        /*FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(CurrentWidget->TakeWidget());
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = true;*/
    }
}

APlayerController* UUIManagerSubsystem::GetOwnerController()
{
    APlayerController* Result = nullptr;
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            Result = PC;
        }
    }
    return Result;
}