#include "SessionsListWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "SessionEntryWidget.h"

void USessionsListWidget::UpdateSessionList(const TArray<class FOnlineSessionSearchResult>& Results)
{
    if (!SessionsListBox)
    {
        UE_LOG(LogTemp, Warning, TEXT("SessionsListBox is not bound in LobbyListWidget"));
        return;
    }
    SessionsListBox->ClearChildren();
    for (const FOnlineSessionSearchResult& Result : Results)
    {
        USessionEntryWidget* Entry = CreateWidget<USessionEntryWidget>(GetWorld(), SessionEntryClass);

        if (Entry)
        {
            Entry->Setup(Result);
            SessionsListBox->AddChild(Entry);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to cast to USessionEntryWidget"));
        }
    }
}
