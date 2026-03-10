#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SessionRequestType.h"

// InGame Widget
#include "CompassWidget.h"
#include "CrossHairWidget.h"
#include "InteractWidget.h"

#include "UIManagerSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionLogicRequested, const FSessionRequest&);
class UInteractWidget;

USTRUCT(BlueprintType)
struct FInGameUISet 
{
	GENERATED_BODY()
	FInGameUISet(UUserWidget* Compass = nullptr, UUserWidget* CrossHair = nullptr, UInteractWidget* Interaction = nullptr)
		: Compass(Compass), CrossHair(CrossHair), Interaction(Interaction)
	{
	}
	void AddToViewPort() 
	{
		Compass->AddToViewport();
		CrossHair->AddToViewport();
		Interaction->AddToViewport();
	}
	UPROPERTY()
	UUserWidget* Compass;
	UPROPERTY()
	UUserWidget* CrossHair;
	UPROPERTY()
	UInteractWidget* Interaction;
};

UCLASS()
class BATTLESKY_API UUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void Subscribe();

	FOnSessionLogicRequested OnSessionLogicRequested;
	UFUNCTION(BlueprintCallable)
	void OnConfirmButtonClicked(const FString& PlayerName);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void OnRefreshButtonClicked() const { OnSessionLogicRequested.Broadcast(CurrentSessionRequest); }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> NameInputWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> SessionListWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> SessionLobbyWidgetClass;
	
	


	// In Game UI (HUD)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FInGameUISet InGameUISets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> CompassWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> CrossHairWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> InteractWidgetClass;




	void ShowInGameUI(APlayerController* Owner);
	void ShowInteractWidget(const bool bShowing, const FText& Text);
	FORCEINLINE bool CanHideInteractionWidget() const {
		if (InGameUISets.Interaction)
		{
			return InGameUISets.Interaction->GetVisibility() == ESlateVisibility::Visible;
		}
		return false;
	};


	void ShowMainMenu(APlayerController* Owner);
	void ShowSessionLobby(APlayerController* Owner, bool bIsHost);

	UFUNCTION(BlueprintCallable)
	void ShowNameInput(bool bIsCreating);

	void StartJoinFlow(const FOnlineSessionSearchResult& TargetSession);
	UFUNCTION(BlueprintCallable)
	void StartLeaveFlow();

	UFUNCTION(BlueprintCallable)
	void BackToMainMenu();

	void SetSessionList(const TArray<FOnlineSessionSearchResult>&);

	void UpdateLobbyPlayerNames();
	UFUNCTION(BlueprintCallable)
	void StartGame();
	

private:
	APlayerController* GetOwnerController();

	void SwitchWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	FSessionRequest CurrentSessionRequest;
	UUserWidget* CurrentWidget = nullptr;
	
	bool bIsCreatingSession;
};
