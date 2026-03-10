#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "SessionEntryWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class BATTLESKY_API USessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()
public :

	void Setup(const FOnlineSessionSearchResult& InSearchResult);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerNumber;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SessionName;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Session;
private:
	UFUNCTION()
	void OnSessionEntryButtonClicked();

	FName TargetSessionName;
	FOnlineSessionSearchResult CachedSearchResult;
};
