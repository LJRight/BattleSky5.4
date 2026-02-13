// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterStateTypes.h"
#include "BattleSkyAnimInstance.generated.h"

UCLASS()
class BATTLESKY_API UBattleSkyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
protected:
	float Delta;
	ACharacter* OwningCharacter;

private:
	void UpdateCharacterInfo();
	void UpdateAimingValues();
	void UpdateLayerValues();
	void UpdateFootIK();

	FVector CalculateAcceleration(const FVector& CurrentVelocity, float DeltaSeconds) const;
	// Character Information
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess="ture"))
	FRotator AimingRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	FVector Velocity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	FVector RelativeVelocityDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	FVector Acceleration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	FVector MovementInput;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	bool IsMoving;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	bool HasMovementInput;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	float MovementInputAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	float AimYawRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information", meta = (AllowPrivateAccess = "ture"))
	float ZoomAmount;


	// State Values
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EMovementState MovementState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EMovementState PrevMovementState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EMovementAction MovementAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	ERotationMode RotationMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EGait Gait;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EStance Stance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EViewMode ViewMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EOverlayState OverlayState;

	// Cached Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached Variables", meta = (AllowPrivateAccess = "true"))
	FVector PreviousVelocity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached Variables", meta = (AllowPrivateAccess = "true"))
	float PreviousAimYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached Variables", meta = (AllowPrivateAccess = "true"))
	FVector2D PlayerMovementInputVector;

	// Aiming Values
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Aiming Values)", meta = (AllowPrivateAccess = "true"))
	FRotator SmoothedAimingRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Aiming Values)", meta = (AllowPrivateAccess = "true"))
	FRotator SpineRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Aiming Values)", meta = (AllowPrivateAccess = "true"))
	FVector2D AimingAngle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Aiming Values)", meta = (AllowPrivateAccess = "true"))
	FVector2D SmoothedAimingAngle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Aiming Values)", meta = (AllowPrivateAccess = "true"))
	float AimSweepTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Aiming Values)", meta = (AllowPrivateAccess = "true"))
	float InputYawOffsetTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Aiming Values)", meta = (AllowPrivateAccess = "true"))
	float ForwardYawTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Aiming Values)", meta = (AllowPrivateAccess = "true"))
	float LeftYawTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Aiming Values)", meta = (AllowPrivateAccess = "true"))
	float RightYawTime;
	
	// User Options
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float AnimatedWalkSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float AnimatedRunSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float AnimatedSprintSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float AnimatedCrouchSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float VelocityBlendInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float GroundedLeanInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float InAirLeanInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float SmoothedAimingRotationInterpSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float InputYawOffsetInterpSpeed;

};
