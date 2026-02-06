#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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
private:
	
	UFUNCTION(Server, Reliable)
	void Server_SendPlayerNameToGameState(const FString& PlayerName);

	void ShowLobbyUI();
};

