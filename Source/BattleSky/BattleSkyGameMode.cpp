// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleSkyGameMode.h"
#include "BattleSkyCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABattleSkyGameMode::ABattleSkyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
