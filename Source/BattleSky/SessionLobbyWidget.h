// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API USessionLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public :
	void UpdateLobbyPlayerNames(const TArray<FString>& PlayerNames);

protected :
	
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* PlayerListBox;
};
