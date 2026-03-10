#include "UIManagerSubsystem.h"
#include "LANSessionSubsystem.h"
#include "SessionsListWidget.h"
#include "Blueprint/UserWidget.h"
#include "BattleSkyGameInstance.h"
#include "SessionLobbyWidget.h"

#include "LobbyPlayerState.h"
#include "LobbyGameState.h"
#include "LobbyPlayerController.h"



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
	OnSessionLogicRequested.Broadcast(CurrentSessionRequest);
}

void UUIManagerSubsystem::ShowInGameUI(APlayerController* Owner)
{
    InGameUISets = FInGameUISet(
        CreateWidget<UCompassWidget>(GetWorld(), CompassWidgetClass),
        CreateWidget<UCrossHairWidget>(GetWorld(), CrossHairWidgetClass),
		CreateWidget<UInteractWidget>(GetWorld(), InteractWidgetClass)
	);
	InGameUISets.AddToViewPort();
	InGameUISets.Interaction->SetVisibility(ESlateVisibility::Hidden);

    /*UCompassWidget* CompassWidget =
        CreateWidget<UCompassWidget>(GetWorld(), CompassWidgetClass);
	CompassWidget->AddToViewport();
	
	UCrossHairWidget* CrossHairWidget =
		CreateWidget<UCrossHairWidget>(GetWorld(), CrossHairWidgetClass);
    CrossHairWidget->AddToViewport();

    UInteractWidget* InteractWidget = 
        CreateWidget<UInteractWidget>(GetWorld(), InteractWidgetClass);
    InteractWidget->AddToViewport();
	InteractWidget->SetVisibility(ESlateVisibility::Hidden);*/
}

void UUIManagerSubsystem::ShowInteractWidget(const bool bShowing, const FText& Text)
{
    if (InGameUISets.Interaction)
	{
        if (bShowing)
        {
            InGameUISets.Interaction->SetText(Text);
            InGameUISets.Interaction->SetVisibility(ESlateVisibility::Visible);
        }
		else
        {
			UE_LOG(LogTemp, Warning, TEXT("Hidden"));
			InGameUISets.Interaction->SetVisibility(ESlateVisibility::Hidden);
        }
    }
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

    if (UWorld* World = GetWorld())
    {
        if (ALobbyGameState* GS = World->GetGameState<ALobbyGameState>())
        {
			GS->OnLobbyPlayerListChanged.AddUObject(this, &UUIManagerSubsystem::UpdateLobbyPlayerNames);
        }
    }
}

void UUIManagerSubsystem::ShowNameInput(bool bIsCreating)
{
    bIsCreatingSession = bIsCreating;
	CurrentSessionRequest = FSessionRequest(bIsCreating ? FSessionRequest::EType::Create : FSessionRequest::EType::Find);
	SwitchWidget(NameInputWidgetClass);
}

void UUIManagerSubsystem::StartJoinFlow(const FOnlineSessionSearchResult& TargetSession)
{
    CurrentSessionRequest = FSessionRequest(TargetSession);
	OnSessionLogicRequested.Broadcast(CurrentSessionRequest);
}

void UUIManagerSubsystem::StartLeaveFlow()
{
	CurrentSessionRequest = FSessionRequest(FSessionRequest::EType::Leave);
    OnSessionLogicRequested.Broadcast(CurrentSessionRequest);
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

void UUIManagerSubsystem::UpdateLobbyPlayerNames()
{
    if(!CurrentWidget || !CurrentWidget->IsA<USessionLobbyWidget>())
    {
		UE_LOG(LogTemp, Warning, TEXT("Current Widget is not SessionLobbyWidget"));
        return;
    }
    if (UWorld* World = GetWorld())
    {
        if (AGameStateBase* GS = World->GetGameState())
        {
            TArray<FString> Names;
            for (APlayerState* PS : GS->PlayerArray)
            {
                if (ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS))
                {
                    Names.Add(LobbyPS->LobbyPlayerName);
                }
            }
            if (USessionLobbyWidget* SessionLobbyWidget = Cast<USessionLobbyWidget>(CurrentWidget))
            {
                SessionLobbyWidget->UpdateLobbyPlayerNames(Names);
            }
        }
    }
}

void UUIManagerSubsystem::StartGame()
{
    if (ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(GetOwnerController()))
    {
		PlayerController->Server_RequestStartGame();
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