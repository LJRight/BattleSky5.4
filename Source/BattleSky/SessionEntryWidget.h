#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "SessionEntryWidget.generated.h"


UCLASS()
class BATTLESKY_API USessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()
public :

	void Setup(const FOnlineSessionSearchResult& InSearchResult);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNumber;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SessionName;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Session;

private:
	UFUNCTION()
	void OnSessionEntryButtonClicked();

	FName TargetSessionName;
	FOnlineSessionSearchResult CachedSearchResult;
};
