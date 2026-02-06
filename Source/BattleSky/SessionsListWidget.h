#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "SessionsListWidget.generated.h"

UCLASS()
class BATTLESKY_API USessionsListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateSessionList(const TArray<FOnlineSessionSearchResult>& Results);

protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* SessionsListBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* RefreshBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* BackBtn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Session")
	TSubclassOf<class USessionEntryWidget> SessionEntryClass;
};
