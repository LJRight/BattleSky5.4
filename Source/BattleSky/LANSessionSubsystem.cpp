// Fill out your copyright notice in the Description page of Project Settings.


#include "LANSessionSubsystem.h"
#include "UIManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MainMenuPlayerController.h"
#include "BattleSkyGameInstance.h"

static FName NAME_SessionLevel(TEXT("/Game/Levels/LobbyLevel"));

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
		FDelegateHandle OnConfirmButtonClickedHandle = 
			UI->OnSessionLogicRequested.AddUObject(this, &ULANSessionSubsystem::OnSessionRequestReceived);
	}
}

void ULANSessionSubsystem::FindSessions()
{
	if (!SessionInterface.IsValid())
	{
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

void ULANSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!SessionInterface.IsValid())
	{
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
	SessionSettings.Set(TEXT("HOST_PLAYER_NAME"),PlayerName,EOnlineDataAdvertisementType::ViaOnlineService);
	
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
		//OnSessionCreated.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create session"));
	}
}



void ULANSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{

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