// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "SessionRequestType.h"
#include "LANSessionSubsystem.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionsFound, const TArray<FOnlineSessionSearchResult>&);
DECLARE_MULTICAST_DELEGATE(FOnSessionCreated);

UCLASS()
class BATTLESKY_API ULANSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void Subscribe();

	void FindSessions();
	void CreateSession();
	void JoinSession(const FOnlineSessionSearchResult& TargetSession);
	void LeaveSession();


	UFUNCTION(BlueprintCallable)
	void OnSessionsFoundFromBP(const TArray<FBlueprintSessionResult>& Results);

	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnLeaveSessionComplete(FName SessionName, bool bWasSuccessful);

	// 1) OnlineSubsystem delegate
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteHandle;

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteHandle;

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteHandle;

	// 2) UI¿¡°Ô »Ñ¸± Ä¿½ºÅÒ delegate
	
	FOnSessionsFound OnSessionsFound;
	FOnSessionCreated OnSessionCreated;

private :
	void OnSessionRequestReceived(const FSessionRequest& Request);

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
