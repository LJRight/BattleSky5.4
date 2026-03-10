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
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float F;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float B;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float L;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
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
struct FTurnInPlace
{
	GENERATED_BODY()
	FTurnInPlace(UAnimSequenceBase* Animation = nullptr, float AnimatedAngle = 0.f, FName SlotName = "", float PlayRate = 0.f, bool ScaleTurnAngle = 0.f)
		: Animation(Animation), AnimatedAngle(AnimatedAngle), SlotName(SlotName), PlayRate(PlayRate), ScaleTurnAngle(ScaleTurnAngle)
	{
	}
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UAnimSequenceBase* Animation;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float AnimatedAngle;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FName SlotName;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float PlayRate;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool ScaleTurnAngle;
};

USTRUCT(BlueprintType)
struct FDynamicMontageParams
{
	GENERATED_BODY()
	FDynamicMontageParams(UAnimSequenceBase* Animation = nullptr, float BlendInTime = 0.f, float BlendOutTime = 0.f, float PlayRate = 0.f, float StartTime = 0.f)
		: Animation(Animation), BlendInTime(BlendInTime), BlendOutTime(BlendOutTime), PlayRate(PlayRate), StartTime(StartTime)
	{
	}
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UAnimSequenceBase* Animation;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float BlendInTime;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float BlendOutTime;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float PlayRate;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float StartTime;
};

USTRUCT(BlueprintType)
struct FLeanAmount
{
	GENERATED_BODY()
	FLeanAmount(float LR = 0, float FB = 0) 
		: LR(LR), FB(FB)
	{
	}
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float LR;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float FB;
	FLeanAmount Interp(const FLeanAmount& Target, float InterpSpeed, float DeltaTime) const
	{
		return FLeanAmount(
			FMath::FInterpTo(this->LR, Target.LR, InterpSpeed, DeltaTime),
			FMath::FInterpTo(this->FB, Target.FB, InterpSpeed, DeltaTime)
		);
	}

};

UENUM(BlueprintType)
enum class EHipsDirection : uint8
{
	F UMETA(DisplayName = "F"),
	B UMETA(DisplayName = "B"),
	RF UMETA(DisplayName = "RF"),
	RB UMETA(DisplayName = "RB"),
	LF UMETA(DisplayName = "LF"),
	LB UMETA(DisplayName = "LB")
};

UCLASS()
class BATTLESKY_API UBattleSkyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayTransition(const FDynamicMontageParams Parameters);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayDynamicTransition(const float ReTriggerDelay, const FDynamicMontageParams Parameters);

	void TurnInPlace(const FRotator TargetRotation, const float PlayRateScale, const float StartTime, const bool OverrideCurrent, bool bRotated90);

	void OnJumped();

// 여기부터 대충 넣고 구현한 것. 정리해야함
	UFUNCTION(BlueprintImplementableEvent)
	FORCEINLINE void OnWeaponChanged();
	UFUNCTION(BlueprintImplementableEvent)
	FORCEINLINE void OnInteraction();
private:
	FTimerHandle JumpTimerHandle;
	FORCEINLINE void ResetJumped() { Jumped = false; };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float PeekingValue;
// 
protected:
	float Delta;
	ACharacter* OwningCharacter;

private:
	// Commone Update
	void UpdateCharacterInfo();
	void UpdateAimingValues();
	void UpdateLayerValues();
	void UpdateFootIK();
	void SetFootLocking(const FName EnableFootIKCurve, const FName FootLockCurve, const FName IKFootBone, float& CurrentFootLockAlpha, FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation);
	void SetFootOffsets(const FName EnableFootIKCurve, const FName IKFootBone, const FName RootBone, FVector& CurrentLocationTarget, FVector& CurrentLocationOffset, FRotator& CurrentRotationOffset);
	void SetFootLockOffsets(FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation);
	void SetPelvisIKOffset(const FVector FootOffset_L_Target, const FVector FootOffset_R_Target);
	void ResetIKOffsets();


	// Grounded
	bool ShouldMoveCheck() const;
	bool PrevShouldMove;
	void UpdateMovementValues();
	FVelocityBlend CalculateVelocityBlend();

	float CalculateStandingPlayRate();
	float CalculateCrouchingPlayRate();

	void UpdateRotationValues();
	EMovementDirection CalculateMovementDirection() const;
	EMovementDirection CalculateQuadrant(const EMovementDirection Current, const float FR_Threshold, const float FL_Threshold, const float BR_Threshold, const float BL_Threshold, const float Buffer, const float Angle) const;
	bool AngleInRange(const float Angle, const float MinAngle, const float MaxAngle, const float Buffer, const bool IncreaseBuffer) const;

	FORCEINLINE bool CanRotateInPlace() const { return RotationMode == ERotationMode::Aiming || ViewMode == EViewMode::FirstPerson; };
	bool CanTurnInPlace() const;
	bool CanDynamicTransition() const;

	void RotateInPlaceCheck();
	void TurnInPlaceCheck();
	
	void DynamicTransitionCheck();
	
	// In Air
	void UpdateInAirValues();
	float CalculateLandPrediction();
	FLeanAmount CalculateInAirLeanAmount();
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	EMovementDirection MovementDirection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	EHipsDirection HipsDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	FVector RelativeAccelerationAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	bool ShouldMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	bool Rotate_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	bool Rotate_R;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float FYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float BYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float LYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Grounded)", meta = (AllowPrivateAccess = "ture"))
	float RYaw;


	// Anim Graph - In Air
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph (In Air)", meta = (AllowPrivateAccess = "ture"))
	bool Jumped;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph (In Air)", meta = (AllowPrivateAccess = "ture"))
	float JumpPlayRate = 1.2f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph (In Air)", meta = (AllowPrivateAccess = "ture"))
	float FallSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph (In Air)", meta = (AllowPrivateAccess = "ture"))
	float LandPrediction;



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

	// Anim Graph - Foot IK
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	float FootLock_L_Alpha;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	float FootLock_R_Alpha;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	FVector FootLock_L_Location;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	FVector FootLock_R_Location;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	FRotator FootLock_L_Rotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	FRotator FootLock_R_Rotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	FVector FootOffset_L_Location;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	FVector FootOffset_R_Location;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	FRotator FootOffset_L_Rotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	FRotator FootOffset_R_Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	FVector PelvisOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph (Foot IK)", meta = (AllowPrivateAccess = "true"))
	float PelvisAlpha;



	// Turn In Place
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float TurnCheckMinAngle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Turn180Threshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float AimYawRateLimit;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float ElapsedDelayTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float MinAngleDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float MaxAngleDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace N_TurnInPlace_L_90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace N_TurnInPlace_R_90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace N_TurnInPlace_L_180;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace N_TurnInPlace_R_180;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace CLF_TurnInPlace_L_90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace CLF_TurnInPlace_R_90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace CLF_TurnInPlace_L_180;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace CLF_TurnInPlace_R_180;


	// Rotate In Place
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotate In Place", meta = (AllowPrivateAccess = "true"))
	float RotateMinThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotate In Place", meta = (AllowPrivateAccess = "true"))
	float RotateMaxThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotate In Place", meta = (AllowPrivateAccess = "true"))
	float AimYawRateMinRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotate In Place", meta = (AllowPrivateAccess = "true"))
	float AimYawRateMaxRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotate In Place", meta = (AllowPrivateAccess = "true"))
	float MinPlayRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotate In Place", meta = (AllowPrivateAccess = "true"))
	float MaxPlayRate;


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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float TriggerPivotSpeedLimit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float FootHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float IK_TraceDistanceAboveFoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Option", meta = (AllowPrivateAccess = "true"))
	float IK_TraceDistanceBelowFoot;


	// Dynamic Additive Transition 
	// ALS 에서 DynamicTransitionCheck 함수 내 이벤트 호출 시 입력 파라미터 보고, 에디터에서 구조체 설정하기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Additive Transition", meta = (AllowPrivateAccess = "true"))
	FDynamicMontageParams DynamicTransition_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Additive Transition", meta = (AllowPrivateAccess = "true"))
	FDynamicMontageParams DynamicTransition_R;

};
