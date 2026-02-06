#include "SessionEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

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
}