// Fill out your copyright notice in the Description page of Project Settings.


#include "LANSessionSubsystem.h"
#include "UIManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MainMenuPlayerController.h"
#include "BattleSkyGameInstance.h"

static FName NAME_SessionLevel(TEXT("/Game/Levels/LobbyLevel"));
static FName NAME_MainLevel(TEXT("/Game/Levels/MainLevel"));

void ULANSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (!OSS)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubsystem not found"));
		return;
	}
	SessionInterface = OSS->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface invalid"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("LANSessionSubsystem Initialized"));

	// 엔진 델리게이트에 연결할 함수 지정
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ULANSessionSubsystem::OnFindSessionsComplete);
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ULANSessionSubsystem::OnCreateSessionComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ULANSessionSubsystem::OnJoinSessionComplete);
}

void ULANSessionSubsystem::Subscribe()
{
	if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("LAN Session Subsystem has completed Event subscribe"));
		FDelegateHandle OnSessionLogicRequestedHandle = 
			UI->OnSessionLogicRequested.AddUObject(this, &ULANSessionSubsystem::OnSessionRequestReceived);

		FDelegateHandle OnJoinSessionRequestedHandle = 
			UI->OnJoinSessionRequested.AddUObject(this, &ULANSessionSubsystem::JoinSession);
	}
}

void ULANSessionSubsystem::FindSessions()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface is InValid"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("LAN Session SubSystem is Finding Sessions..."));
	
	/* 세션 검색 동작이 C++ 로 구현 시 제대로 동작하지 않아, 일단은 MainMenuPlayerController BP 에 위임하여 FindSessions 노드로 검색 후,
	   해당 결과를 LANSessionSubsystem 에 전달하는 구조로 설계 */
	if (AMainMenuPlayerController* PC = Cast<AMainMenuPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Delegating FindSessions to MainMenuPlayerController BP..."));
		PC->SearchLocalSession_BP();
	}
	/*
	OnFindSessionsCompleteHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 50;
	SessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	*/
}

void ULANSessionSubsystem::OnSessionsFoundFromBP(const TArray<FBlueprintSessionResult>& Results)
{
	UE_LOG(LogTemp, Warning, TEXT("Finding Sessions Completed : Number (%d)"), Results.Num());

	TArray<FOnlineSessionSearchResult> SearchResults;
	SearchResults.Reserve(Results.Num());
	for (const FBlueprintSessionResult& BPResult : Results)
	{
		SearchResults.Add(BPResult.OnlineResult);
	}
	OnSessionsFound.Broadcast(SearchResults);
}

// C++ 로직, 정상적으로 동작하지 않음
void ULANSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface is InValid"));
		return;
	}
	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
	if (bWasSuccessful && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Finding Sessions Completed : Number (%d)"), SessionSearch->SearchResults.Num());
		OnSessionsFound.Broadcast(SessionSearch->SearchResults);
	}
}

void ULANSessionSubsystem::CreateSession()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface is InValid"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("LAN Session SubSystem is Creating Sessions..."));
	OnCreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 4;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;

	// NaemInput Widget 에서 입력한 플레이어 이름을 세션 설정에 추가
	FString PlayerName = "NoName";
	if (UBattleSkyGameInstance* GI = Cast<UBattleSkyGameInstance>(GetGameInstance()))
	{
		PlayerName = GI->PlayerName;
	}
	SessionSettings.Set(TEXT("HOST_PLAYER_NAME"), PlayerName, EOnlineDataAdvertisementType::ViaOnlineService);
	
	if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
}

void ULANSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session %s created!"), *SessionName.ToString());
		UGameplayStatics::OpenLevel(GetWorld(), NAME_SessionLevel, true, "listen");
		// OnSessionCreated.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create session"));
	}
}

void ULANSessionSubsystem::JoinSession(const FOnlineSessionSearchResult& TargetSession)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface is InValid"));
		return;
	}
	OnJoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
	
	bool bJoinStarted = SessionInterface->JoinSession(0, NAME_GameSession, TargetSession);

	if (!bJoinStarted)
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinSession: Failed to start joining session"));
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);
	}
}

void ULANSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface is InValid"));
		return;
	}

	// Delegate 해제
	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString ConnectString;
		if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
		{
			UE_LOG(LogTemp, Log, TEXT("Joining session at %s"), *ConnectString);

			// 실제 서버로 접속
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("JoinSession: Could not get connection string"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinSession failed with result: %d"), (int32)Result);
	}
}

void ULANSessionSubsystem::LeaveSession()
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// 현재 로컬 플레이어 세션 이름 가져오기
	FName SessionName = NAME_GameSession;

	FOnDestroySessionCompleteDelegate DestroyDelegate;
	DestroyDelegate.BindUObject(this, &ULANSessionSubsystem::OnLeaveSessionComplete);

	SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroyDelegate);

	SessionInterface->DestroySession(SessionName);
}

void ULANSessionSubsystem::OnLeaveSessionComplete(FName SessionName, bool bWasSuccessful)
{
	 if (bWasSuccessful)
	 {
        UE_LOG(LogTemp, Warning, TEXT("Left session %s successfully"), *SessionName.ToString());

		// 로비 UI로 돌아가기
      /*  if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
        {
            UI->ShowMainMenu();
        }*/

		// 레벨 이동: 로컬 클라이언트를 로비 레벨로 이동
		UGameplayStatics::OpenLevel(GetWorld(), NAME_MainLevel);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to leave session %s"), *SessionName.ToString());
    }
}
void ULANSessionSubsystem::OnSessionRequestReceived(bool bIsCreating)
{
	bIsCreating ? CreateSession() : FindSessions();
}

void ULANSessionSubsystem::Deinitialize()
{
    Super::Deinitialize();
	// 공유 포인터 참조 해제
	SessionInterface.Reset();

	UE_LOG(LogTemp, Warning, TEXT("LANSessionSubsystem Deinitialized"));
}