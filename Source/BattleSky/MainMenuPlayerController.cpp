// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuPlayerController.h"
#include "UIManagerSubsystem.h"

static FName NAME_SessionLevel(TEXT("/Game/Levels/LobbyLevel"));

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->ShowMainMenu(this);
	}
}