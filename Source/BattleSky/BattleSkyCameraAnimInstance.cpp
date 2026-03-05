// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSkyCameraAnimInstance.h"
#include "BattleSkyCharacter.h"

void UBattleSkyCameraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (ControlledPawn)
	{
		UpdateCharacterInfo();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Camera Anim hasn't Pawn"));
	}
}

void UBattleSkyCameraAnimInstance::UpdateCharacterInfo()
{
	if(ABattleSkyCharacter* BattleSkyCharacter = Cast<ABattleSkyCharacter>(ControlledPawn))
	{
		RightShoulder = BattleSkyCharacter->RightShoulder;
		MovementState = BattleSkyCharacter->MovementState;
		PrevMovementState = BattleSkyCharacter->PrevMovementState;
		MovementAction = BattleSkyCharacter->MovementAction;
		Gait = BattleSkyCharacter->Gait;
		Stance = BattleSkyCharacter->Stance;
		ViewMode = BattleSkyCharacter->ViewMode;
	}
}