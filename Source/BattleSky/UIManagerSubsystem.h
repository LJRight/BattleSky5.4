#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SessionRequestType.h"
#include "UIManagerSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionLogicRequested, const FSessionRequest&);

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
	TSubclassOf<UUserWidget> CompassWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> CrossHairWidgetClass;

	void ShowInGameUI(APlayerController* Owner);

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
