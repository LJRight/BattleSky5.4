#pragma once

#include "CoreMinimal.h"
#include "BPLobbyInfo.generated.h"

USTRUCT(BlueprintType)
struct FBPLobbyInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString SessionName;

    UPROPERTY(BlueprintReadOnly)
    FString SessionID;

    UPROPERTY(BlueprintReadOnly)
    int32 MaxPlayers;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentPlayers;

    UPROPERTY(BlueprintReadOnly)
    int32 Ping;
};
