#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SessionRequestType.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ALobbyPlayerController();
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_RequestStartGame();
private:
	
	UFUNCTION(Server, Reliable)
	void Server_SendLobbyPlayerName(const FString& PlayerName);

	/*UFUNCTION(Server, Reliable)
	void Server_LeaveSession(const FSessionRequest& Request);*/


	void ShowLobbyUI();
};