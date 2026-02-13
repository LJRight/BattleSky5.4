// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterStateTypes.h"
#include "BattleSkyAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_BODY()
	FVelocityBlend(float F = 0.f, float B = 0.f, float L = 0.f, float R = 0.f)
		: F(F), B(B), L(L), R(R) 
	{
	}
	float F;
	float B;
	float L;
	float R;
	FVelocityBlend Interp(const FVelocityBlend& Target, float InterpSpeed, float DeltaTime) const
	{
		return FVelocityBlend(
			FMath::FInterpTo(this->F, Target.F, InterpSpeed, DeltaTime),
			FMath::FInterpTo(this->B, Target.B, InterpSpeed, DeltaTime),
			FMath::FInterpTo(this->L, Target.L, InterpSpeed, DeltaTime),
			FMath::FInterpTo(this->R, Target.R, InterpSpeed, DeltaTime)
		);
	}
};

USTRUCT(BlueprintType)
struct FLeanAmount
{
	GENERATED_BODY()
	FLeanAmount(float LR = 0, float FB = 0) 
		: LR(LR), FB(FB)
	{
	}
	float LR;
	float FB;
	FLeanAmount Interp(const FLeanAmount& Target, float InterpSpeed, float DeltaTime) const
	{
		return FLeanAmount(
			FMath::FInterpTo(this->LR, Target.LR, InterpSpeed, DeltaTime),
			FMath::FInterpTo(this->FB, Target.FB, InterpSpeed, DeltaTime)
		);
	}

};

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

	bool ShouldMoveCheck() const;

	bool PrevShouldMove;

	void UpdateMovementValues();
	FVelocityBlend CalculateVelocityBlend();

	float CalculateStandingPlayRate();
	float CalculateCrouchingPlayRate();

	void UpdateRotationValues();

	// FVector CalculateAcceleration(const FVector& CurrentVelocity, float DeltaSeconds) const;

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

	// Anim Graph - Grounded
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	FVector RelativeAccelerationAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	bool ShouldMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	bool Rotate_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	bool Rotate_R;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	bool Pivot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float RotateRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float RotationScale;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float DiagonalScaleAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float WalkRunBlend;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float StandingPlayRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float CrouchingPlayRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float StrideBlend;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	FVelocityBlend VelocityBlend;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	FLeanAmount	LeanAmount;




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
	
	// Anim Graph - Layer Blending
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	int OverlayOverrideState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Enable_AimOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float BasePose_N;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float BasePose_CLF;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Arm_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Arm_L_Add;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Arm_L_LS;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Arm_L_MS;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Arm_R;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Arm_R_Add;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Arm_R_LS;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Arm_R_MS;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Hand_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Hand_R;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Legs;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Legs_Add;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Pelvis;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Spine;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Spine_Add;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Head;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Head_Add;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Enable_HandIK_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Layer Blending)", meta = (AllowPrivateAccess = "true"))
	float Enable_HandIK_R;


	// Turn In Place
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float TurnCheckMinAngle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Turn180Threshold;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float AimYawRateLimit;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float ElapsedDelayTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float MinAngleDelay;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float MaxAngleDelay;


	// Blend Curves
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blend Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DiagonalScaleAmountCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blend Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlend_N_Walk;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blend Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlend_N_Run;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blend Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlend_C_Walk;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blend Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* LandPredictionCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blend Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* LeanInAirCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blend Curves", meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffset_FB;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blend Curves", meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffset_LR;


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
