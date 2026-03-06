// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterStateTypes.h"
#include "BattleSkyCameraAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API UBattleSkyCameraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	FORCEINLINE void SetControlledPawn(APawn* NewPawn) { ControlledPawn = NewPawn; }
	FORCEINLINE void SetOwningController(APlayerController* NewController) { OwningController = NewController; }
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:

	void UpdateCharacterInfo();
	APawn* ControlledPawn;
	APlayerController* OwningController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "true"))
	bool RightShoulder;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "true"))
	EMovementState MovementState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "true"))
	EMovementState PrevMovementState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "true"))
	EMovementAction MovementAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "true"))
	EGait Gait;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "true"))
	EStance Stance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "true"))
	EViewMode ViewMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "true"))
	ERotationMode RotationMode;
};
