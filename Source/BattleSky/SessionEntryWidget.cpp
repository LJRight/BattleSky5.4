#include "SessionEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UIManagerSubsystem.h"

void USessionEntryWidget::Setup(const FOnlineSessionSearchResult& InSearchResult)
{
	CachedSearchResult = InSearchResult;
	FString PlayerCount = FString::Printf(TEXT("%d / %d"), 
		CachedSearchResult.Session.SessionSettings.NumPublicConnections - CachedSearchResult.Session.NumOpenPublicConnections,
		CachedSearchResult.Session.SessionSettings.NumPublicConnections);

	PlayerNumber->SetText(FText::FromString(PlayerCount));

	FString HostName;
	if (CachedSearchResult.Session.SessionSettings.Get(TEXT("HOST_PLAYER_NAME"), HostName))
	{
		SessionName->SetText(FText::FromString(HostName + "'s Lobby"));
	}

	//TargetSessionName = CachedSearchResult.Session
	if (Btn_Session)
	{
		Btn_Session->OnClicked.Clear();
		Btn_Session->OnClicked.AddDynamic(this, &USessionEntryWidget::OnSessionEntryButtonClicked);
	}
}

void USessionEntryWidget::OnSessionEntryButtonClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (UUIManagerSubsystem* UIManager = World->GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Join Button Clicked, And attempt to Call UI Manager for Join that Session"));
			UIManager->StartJoinFlow(CachedSearchResult);
		}
	}
}