#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIManagerSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionLogicRequested, const bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinSessionRequested, const FOnlineSessionSearchResult&);

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
	FORCEINLINE void OnRefreshButtonClicked() const { OnSessionLogicRequested.Broadcast(false); }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> NameInputWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> SessionListWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> SessionLobbyWidgetClass;

	void ShowMainMenu(APlayerController* Owner);
	void ShowSessionLobby(APlayerController* Owner, bool bIsHost);

	UFUNCTION(BlueprintCallable)
	void StartCreateFlow();
	UFUNCTION(BlueprintCallable)
	void StartFindFlow();
	void StartJoinFlow(const FOnlineSessionSearchResult& TargetSession) const { OnJoinSessionRequested.Broadcast(TargetSession); }
	UFUNCTION(BlueprintCallable)
	void StartLeaveFlow();
	FOnJoinSessionRequested OnJoinSessionRequested;

	UFUNCTION(BlueprintCallable)
	void BackToMainMenu();

	void SetSessionList(const TArray<FOnlineSessionSearchResult>&);

	void UpdateLobbyPlayerNames(const TArray<FString>& PlayerNames);

private:
	APlayerController* GetOwnerController();

	void SwitchWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	UUserWidget* CurrentWidget = nullptr;
	
	bool bIsCreatingSession;
};
