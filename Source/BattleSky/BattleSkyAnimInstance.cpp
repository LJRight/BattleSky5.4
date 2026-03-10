// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSkyAnimInstance.h"
#include "BattleSkyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

const static FName NAME_Mask_AimOffset("Mask_AimOffset");
const static FName NAME_BasePose_N("BasePose_N");
const static FName NAME_BasePose_CLF("BasePose_CLF");

const static FName NAME_Layering_Spine_Add("Layering_Spine_Add");
const static FName NAME_Layering_Head_Add("Layering_Head_Add");
const static FName NAME_Layering_Arm_L_Add("Layering_Arm_L_Add");
const static FName NAME_Layering_Arm_R_Add("Layering_Arm_R_Add");

const static FName NAME_Layering_Hand_R("Layering_Hand_R");
const static FName NAME_Layering_Hand_L("Layering_Hand_L");

const static FName NAME_Enable_HandIK_L("Enable_HandIK_L");
const static FName NAME_Layering_Arm_L("Layering_Arm_L");

const static FName NAME_Enable_HandIK_R("Enable_HandIK_R");
const static FName NAME_Layering_Arm_R("Layering_Arm_R");

const static FName NAME_Layering_Arm_L_LS("Layering_Arm_L_LS");
const static FName NAME_Layering_Arm_R_LS("Layering_Arm_R_LS");

const static FName NAME_Weight_Gait("Weight_Gait");

const static FName NAME_Enable_Transition("Enable_Transition");

const static FName NAME_Enable_FootIK_L("Enable_FootIK_L");
const static FName NAME_FootLock_L("FootLock_L");

const static FName NAME_Enable_FootIK_R("Enable_FootIK_R");
const static FName NAME_FootLock_R("FootLock_R");

// 본 이름에 맞게 소켓 이름 바꿀것?
const static FName NAME_IK_Foot_L("ik_foot_l");
const static FName NAME_IK_Foot_R("ik_foot_r");
const static FName NAME_VB_Foot_Target_L("VB foot_target_l");
const static FName NAME_VB_Foot_Target_R("VB foot_target_r");

const static FName NAME_RootBone("root");

void UBattleSkyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningCharacter = Cast<ACharacter>(TryGetPawnOwner());
}

void UBattleSkyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	Delta = DeltaSeconds;
	if (Delta != 0.f)
	{
		if (OwningCharacter)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Rotation Curve Value : %f"), GetCurveValue(FName("RotationAmount")));
			UpdateCharacterInfo();
			UpdateAimingValues();
			UpdateLayerValues();
			UpdateFootIK();


			if (MovementState == EMovementState::Grounded)
			{
				ShouldMove = ShouldMoveCheck();
				bool bChanged = ShouldMove == PrevShouldMove;

				// 움직임 여부 상태가 변화했는지,
				if (bChanged)
				{
					// False -> True 로 변했다면 (움직이기 시작했다면)
					if (ShouldMove)
					{
						ElapsedDelayTime = 0.f;
						Rotate_L = false;
						Rotate_R = false;
					}
				}
				
				// 움직이고 있는 동안
				if (ShouldMove)
				{
					UpdateMovementValues();
					UpdateRotationValues();
				}

				// 멈춰 있는 동안 (여기에서 1인칭(RotateInPlace) / 3인칭(TurnInPlace) 여부를 체크하는 로직. 애니메이션 인스턴스에서 하는 게 맞는지? 
				// 
				else
				{
					if(CanRotateInPlace())
					{
						RotateInPlaceCheck();
					}
					else
					{
						Rotate_L = false;
						Rotate_R = false;
					}
					
					/*if(CanTurnInPlace())
					{
						TurnInPlaceCheck();
					}
					else
					{
						ElapsedDelayTime = 0.f;
					}*/

					if(CanDynamicTransition())
					{
						DynamicTransitionCheck();
					}
				}

				PrevShouldMove = ShouldMove;
			}
			else if(MovementState == EMovementState::InAir)
			{

			}
		}
	}
}

// 로컬 캐릭터의 정보를 가져온다 (리플리케이션 되는 값들)
void UBattleSkyAnimInstance::UpdateCharacterInfo()
{
	Velocity = OwningCharacter->GetVelocity();
	// 현재 프레임과 이전 프레임의 속도 차이를 이용해 가속도를 계산한다
	Acceleration = (Velocity - PreviousVelocity) / Delta;
	MovementInput = OwningCharacter->GetCharacterMovement()->GetCurrentAcceleration();
	Speed = Velocity.Size();
	IsMoving = Speed > 1.0f;
	MovementInputAmount = MovementInput.Size() / OwningCharacter->GetCharacterMovement()->GetMaxAcceleration();
	HasMovementInput = MovementInputAmount > 0;


	// AimingRotation = OwningCharacter->GetControlRotation();
	// AimYawRate = FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / Delta);
	if(ABattleSkyCharacter* BattleSkyCharacter = Cast<ABattleSkyCharacter>(OwningCharacter))
	{
		AimingRotation = BattleSkyCharacter->Replicated_AimingRotation;
		AimYawRate = FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / Delta);

		MovementState = BattleSkyCharacter->MovementState;
		PrevMovementState = BattleSkyCharacter->PrevMovementState;
		MovementAction = BattleSkyCharacter->MovementAction;
		RotationMode = BattleSkyCharacter->RotationMode;
		Gait = BattleSkyCharacter->Gait;
		Stance = BattleSkyCharacter->Stance;
		OverlayState = BattleSkyCharacter->OverlayState;

		PeekingValue = BattleSkyCharacter->PeekingValue;
	}
	// 현재 프레임의 속도와 조준 회전값을 이전 프레임의 값으로 저장한다. 다음 프레임에서 이 값을 이용해 가속도와 조준 회전 속도를 계산할 것이다
	PreviousVelocity = Velocity;
	PreviousAimYaw = AimingRotation.Yaw;
}

void UBattleSkyAnimInstance::UpdateAimingValues()
{
	SmoothedAimingRotation = FMath::RInterpTo(SmoothedAimingRotation, AimingRotation, Delta, SmoothedAimingRotationInterpSpeed);

	const FRotator ActorRotation = OwningCharacter->GetActorRotation();

	const FRotator DeltaAimingRotation = UKismetMathLibrary::NormalizedDeltaRotator(AimingRotation, ActorRotation);
	AimingAngle = FVector2D(DeltaAimingRotation.Yaw, DeltaAimingRotation.Pitch);

	const FRotator DeltaSmoothedAimingRotation = UKismetMathLibrary::NormalizedDeltaRotator(SmoothedAimingRotation, ActorRotation);
	SmoothedAimingAngle = FVector2D(DeltaSmoothedAimingRotation.Yaw, DeltaSmoothedAimingRotation.Pitch);

	if(RotationMode != ERotationMode::VelocityDirection)
	{
		AimSweepTime = FMath::GetMappedRangeValueClamped(FVector2D(-90.f, 90.f), FVector2D(1.f, 0.f), AimingAngle.Y);

		// Use the Aiming Yaw Angle divided by the number of spine + pelvis bones to get the amount of 
		// spine rotation needed to remain facing the camera direction
		SpineRotation = FRotator(0.f, AimingAngle.X / 4.0f, 0.f);
	}
	const float YawTime = FMath::Abs(SmoothedAimingAngle.X);
	LeftYawTime = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 180.f), FVector2D(.5f, 0.f), YawTime);
	RightYawTime = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 180.f), FVector2D(.5f, 1.f), YawTime);
	ForwardYawTime = FMath::GetMappedRangeValueClamped(FVector2D(-180.f, 180.f), FVector2D(0.f, 1.f), SmoothedAimingAngle.X);
}

void UBattleSkyAnimInstance::UpdateLayerValues()
{
	// Get the Aim Offset weight by getting the opposite of the Aim Offset Mask
	Enable_AimOffset = FMath::Lerp(1.f, 0.f, GetCurveValue(NAME_Mask_AimOffset));

	// Set the Base Pose weights
	GetCurveValue(NAME_BasePose_N, BasePose_N);
	GetCurveValue(NAME_BasePose_CLF, BasePose_CLF);

	// Set the Additive amount weights for each body part
	GetCurveValue(NAME_Layering_Spine_Add, Spine_Add);
	GetCurveValue(NAME_Layering_Head_Add, Head_Add);
	GetCurveValue(NAME_Layering_Arm_L_Add, Arm_L_Add);
	GetCurveValue(NAME_Layering_Arm_R_Add, Arm_R_Add);

	// Set the Hand Override weights
	GetCurveValue(NAME_Layering_Hand_R, Hand_R);
	GetCurveValue(NAME_Layering_Hand_L, Hand_L);

	// Blend and set the Hand IK weights to ensure they only are weighted if allowed by the Arm layers
	Enable_HandIK_L = FMath::Lerp(0.f, GetCurveValue(NAME_Enable_HandIK_L), GetCurveValue(NAME_Layering_Arm_L));
	Enable_HandIK_R = FMath::Lerp(0.f, GetCurveValue(NAME_Enable_HandIK_R), GetCurveValue(NAME_Layering_Arm_R));

	// Set whether the arms should blend in mesh space or local space. The Mesh space weight will always be 1 unless the Local Space (LS) curve is fully weighted.
	GetCurveValue(NAME_Layering_Arm_L_LS, Arm_L_LS);
	Arm_L_MS = 1 - FMath::Floor(Arm_L_LS);
	GetCurveValue(NAME_Layering_Arm_R_LS, Arm_R_LS);
	Arm_R_MS = 1 - FMath::Floor(Arm_R_LS);
}

void UBattleSkyAnimInstance::UpdateFootIK()
{
	SetFootLocking(NAME_Enable_FootIK_L, NAME_FootLock_L, NAME_IK_Foot_L, FootLock_L_Alpha, FootLock_L_Location, FootLock_L_Rotation);
	SetFootLocking(NAME_Enable_FootIK_R, NAME_FootLock_R, NAME_IK_Foot_R, FootLock_R_Alpha, FootLock_R_Location, FootLock_R_Rotation);
	if(MovementState == EMovementState::InAir)
	{
		SetPelvisIKOffset(FVector::ZeroVector, FVector::ZeroVector);
		ResetIKOffsets();
	}
	else
	{
		FVector FootOffset_L_Target;
		FVector FootOffset_R_Target;
		SetFootOffsets(NAME_Enable_FootIK_L, NAME_IK_Foot_L, NAME_RootBone, FootOffset_L_Target, FootOffset_L_Location, FootOffset_L_Rotation);
		SetFootOffsets(NAME_Enable_FootIK_R, NAME_IK_Foot_R, NAME_RootBone, FootOffset_R_Target, FootOffset_R_Location, FootOffset_R_Rotation);
		SetPelvisIKOffset(FootOffset_L_Target, FootOffset_R_Target);
	}
}

void UBattleSkyAnimInstance::SetFootLocking(const FName EnableFootIKCurve, const FName FootLockCurve, const FName IKFootBone, float& CurrentFootLockAlpha, FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation)
{
	if (GetCurveValue(EnableFootIKCurve) > 0.f)
	{
		const float FootLockCurveValue = GetCurveValue(FootLockCurve);
		
		if (FootLockCurveValue >= 0.99f || FootLockCurveValue < CurrentFootLockAlpha)
		{
			CurrentFootLockAlpha = FootLockCurveValue;
		}

		if (CurrentFootLockAlpha > 0.99f)
		{
			const FTransform TargetTransform = GetOwningComponent()->GetSocketTransform(IKFootBone, ERelativeTransformSpace::RTS_Component);
			CurrentFootLockLocation = TargetTransform.GetLocation();
			CurrentFootLockRotation = TargetTransform.GetRotation().Rotator();
		}

		if(CurrentFootLockAlpha > 0.f)
		{
			CurrentFootLockAlpha = FMath::Clamp(CurrentFootLockAlpha, 0.f, 1.f);
			SetFootLockOffsets(CurrentFootLockLocation, CurrentFootLockRotation);
		}
	}
}

void UBattleSkyAnimInstance::SetFootOffsets(const FName EnableFootIKCurve, const FName IKFootBone, const FName RootBone, FVector& CurrentLocationTarget, FVector& CurrentLocationOffset, FRotator& CurrentRotationOffset)
{
	if(GetCurveValue(EnableFootIKCurve) > 0.f)
	{
		const FVector IKFootBoneLocation = GetOwningComponent()->GetSocketLocation(IKFootBone);
		const FVector RootBoneLocation = GetOwningComponent()->GetSocketLocation(RootBone);
		const FVector IKFootFloorLocation = FVector(IKFootBoneLocation.X, IKFootBoneLocation.Y, RootBoneLocation.Z);

		const FVector TraceStartLocation = IKFootFloorLocation + FVector(0.f, 0.f, IK_TraceDistanceAboveFoot);
		const FVector TraceEndLocation = IKFootFloorLocation - FVector(0.f, 0.f, IK_TraceDistanceBelowFoot);

		FHitResult FootIK_TraceResult;
		FCollisionQueryParams FootIK_TraceParams = FCollisionQueryParams(FName(TEXT("FootIKTrace")), false, OwningCharacter);

		GetWorld()->LineTraceSingleByChannel(
			FootIK_TraceResult,
			TraceStartLocation,
			TraceEndLocation,
			ECC_Visibility
		);

		FVector ImpactPoint;
		FVector ImpactNormal;

		FRotator TargetRotationOffset;

		if (UCharacterMovementComponent* CharacterMovement = OwningCharacter->GetCharacterMovement())
		{
			if(CharacterMovement->IsWalkable(FootIK_TraceResult))
			{
				ImpactPoint = FootIK_TraceResult.ImpactPoint;
				ImpactNormal = FootIK_TraceResult.ImpactNormal;

				CurrentLocationTarget = (ImpactNormal * FootHeight + ImpactPoint) - (IKFootFloorLocation + FVector(0.f, 0.f, 1.f) * FootHeight);
				TargetRotationOffset = FRotator(FMath::Atan2(ImpactNormal.Y, ImpactNormal.Z), 0.f, -FMath::Atan2(ImpactNormal.X, ImpactNormal.Z));
			}
		}
		
		const float LocationOffsetInterpSpeed = CurrentLocationOffset.Z > CurrentLocationTarget.Z ? 30.f : 15.f;
		CurrentLocationOffset = FMath::VInterpTo(CurrentLocationOffset, CurrentLocationTarget, Delta, LocationOffsetInterpSpeed);

		CurrentRotationOffset = FMath::RInterpTo(CurrentRotationOffset, TargetRotationOffset, Delta, 30.f);
	}
	else
	{
		CurrentLocationOffset = FVector::ZeroVector;
		CurrentRotationOffset = FRotator::ZeroRotator;
	}
}

void UBattleSkyAnimInstance::SetFootLockOffsets(FVector& LocalLocation, FRotator& LocalRotation)
{
	const UCharacterMovementComponent* CharacterMovement = OwningCharacter->GetCharacterMovement();
	if (!CharacterMovement)
	{
		return;
	}

	FRotator RotationDifference;
	if (CharacterMovement->IsMovingOnGround())
	{
		RotationDifference = OwningCharacter->GetActorRotation() - CharacterMovement->GetLastUpdateRotation();
	}
	const FVector LocationDifference = GetOwningComponent()->GetComponentRotation().UnrotateVector(Velocity * Delta);
	LocalLocation = (LocalLocation - LocationDifference).RotateAngleAxis(RotationDifference.Yaw, FVector(0.f, 0.f, -1.f));
	LocalRotation -= RotationDifference;
}

void UBattleSkyAnimInstance::SetPelvisIKOffset(const FVector FootOffset_L_Target, const FVector FootOffset_R_Target)
{
	PelvisAlpha = (GetCurveValue(NAME_Enable_FootIK_L) + GetCurveValue(NAME_Enable_FootIK_R)) / 2;
	if (PelvisAlpha > 0.f)
	{
		const FVector PelvisTarget = FootOffset_L_Target.Z < FootOffset_R_Target.Z ? FootOffset_L_Target : FootOffset_R_Target;
		
		const float PelvisOffsetInterpSpeed = PelvisTarget.Z > PelvisOffset.Z ? 10.f : 15.f;
		PelvisOffset = FMath::VInterpTo(PelvisOffset, PelvisTarget, Delta, PelvisOffsetInterpSpeed);
	}
	else
	{
		PelvisOffset = FVector::ZeroVector;
	}
}

void UBattleSkyAnimInstance::ResetIKOffsets()
{
	FootOffset_L_Location = FMath::VInterpTo(FootOffset_L_Location, FVector::ZeroVector, Delta, 15.f);
	FootOffset_R_Location = FMath::VInterpTo(FootOffset_R_Location, FVector::ZeroVector, Delta, 15.f);
	FootOffset_L_Rotation = FMath::RInterpTo(FootOffset_L_Rotation, FRotator::ZeroRotator, Delta, 15.f);
	FootOffset_R_Rotation = FMath::RInterpTo(FootOffset_R_Rotation, FRotator::ZeroRotator, Delta, 15.f);
}

bool UBattleSkyAnimInstance::ShouldMoveCheck() const
{
	return (IsMoving && HasMovementInput) || Speed > 150.f;
}

void UBattleSkyAnimInstance::UpdateMovementValues()
{
	// Interp and set the Velocity Blend
	VelocityBlend = VelocityBlend.Interp(CalculateVelocityBlend(), VelocityBlendInterpSpeed, Delta);

	// Set the Diagnal Scale Amount
	DiagonalScaleAmount = DiagonalScaleAmountCurve->GetFloatValue(FMath::Abs(VelocityBlend.F + VelocityBlend.B));

	// Calculate the Relative Acceleration Amount. This value represents the current amount of acceleration / deceleration relative to the actor rotation
	// It is normalized to a range of -1 to 1 so that -1 equals the Max Braking Deceleration, and 1 equals the Max Acceleration of the Character Movement Component
	const float MaxSize = Acceleration.Dot(Velocity) > 0.f ? 
		OwningCharacter->GetCharacterMovement()->GetMaxAcceleration() : OwningCharacter->GetCharacterMovement()->GetMaxBrakingDeceleration();
	RelativeAccelerationAmount = OwningCharacter->GetActorRotation().UnrotateVector(Acceleration.GetClampedToMaxSize(MaxSize) / MaxSize);
	
	LeanAmount = LeanAmount.Interp(FLeanAmount(RelativeAccelerationAmount.Y, RelativeAccelerationAmount.X), GroundedLeanInterpSpeed, Delta);

	WalkRunBlend = (Gait == EGait::Walking) ? 0.f : 1.f;

	StrideBlend = FMath::Lerp(
		FMath::Lerp(
			StrideBlend_N_Walk->GetFloatValue(Speed),
			StrideBlend_N_Run->GetFloatValue(Speed),
			FMath::Clamp(GetCurveValue(NAME_Weight_Gait) - 1.f, 0.f, 1.f)
		),
		StrideBlend_C_Walk->GetFloatValue(Speed),
		GetCurveValue(NAME_BasePose_CLF)
	);

	StandingPlayRate = CalculateStandingPlayRate();
	CrouchingPlayRate = CalculateCrouchingPlayRate();
}

FVelocityBlend UBattleSkyAnimInstance::CalculateVelocityBlend()
{
	const FVector LocalRelativeVelocityDirection = OwningCharacter->GetActorRotation().UnrotateVector(Velocity.GetSafeNormal(.1f));

	const float Sum = FMath::Abs(LocalRelativeVelocityDirection.X) + FMath::Abs(LocalRelativeVelocityDirection.Y) + FMath::Abs(LocalRelativeVelocityDirection.Z);
	const FVector RelativeDirection = LocalRelativeVelocityDirection / Sum;

	return FVelocityBlend(
		FMath::Clamp(RelativeDirection.X, 0.f, 1.f),
		FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.f, 0.f)),
		FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.f, 0.f)),
		FMath::Clamp(RelativeDirection.Y, 0.f, 1.f)
	);
}

float UBattleSkyAnimInstance::CalculateStandingPlayRate()
{
	const float WalkRunPlayRate = FMath::Lerp(
		Speed / AnimatedWalkSpeed,
		Speed / AnimatedRunSpeed,
		FMath::Clamp(GetCurveValue(NAME_Weight_Gait) - 1.f, 0.f, 1.f)
	);
	const float SprintPlayRate = FMath::Lerp(
		WalkRunPlayRate,
		Speed / AnimatedSprintSpeed,
		FMath::Clamp(GetCurveValue(NAME_Weight_Gait) - 2.f, 0.f, 1.f)
	);
	float Result;
	Result = FMath::Clamp(SprintPlayRate / StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.f, 3.f);
	return Result;
}

float UBattleSkyAnimInstance::CalculateCrouchingPlayRate()
{
	float Result;
	Result = FMath::Clamp(Speed / AnimatedCrouchSpeed / StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.f, 2.f);
	return Result;
}

void UBattleSkyAnimInstance::UpdateRotationValues()
{
	if (ABattleSkyCharacter* BattleSkyCharacter = Cast<ABattleSkyCharacter>(OwningCharacter))
	{
		MovementDirection = BattleSkyCharacter->Replicated_MovementDirection;
	}
}

EMovementDirection UBattleSkyAnimInstance::CalculateMovementDirection() const
{
	EMovementDirection Result;
	if(Gait == EGait::Sprinting)
	{
		Result = EMovementDirection::Forward;
	}
	else
	{
		const float RotationDelta = FMath::FindDeltaAngleDegrees(AimingRotation.Yaw, Velocity.Rotation().Yaw);
		Result = CalculateQuadrant(
			MovementDirection,
			// 아래 네 값은 애니메이션 전환 간 임계값으로 변수로 설정하여 조정할 수 있다
			70.f,
			-70.f,
			110.f,
			-110.f,
			5.f,
			RotationDelta
		);
	}
	return Result;
}

EMovementDirection UBattleSkyAnimInstance::CalculateQuadrant(
	const EMovementDirection Current, 
	const float FR_Threshold,
	const float FL_Threshold, 
	const float BR_Threshold, 
	const float BL_Threshold, 
	const float Buffer,
	const float Angle) const
{
	if (AngleInRange(Angle, FL_Threshold, FR_Threshold, Buffer, Current != EMovementDirection::Forward || Current != EMovementDirection::Backward))
	{
		return EMovementDirection::Forward;
	}
	else if (AngleInRange(Angle, FR_Threshold, BR_Threshold, Buffer, Current != EMovementDirection::Right || Current != EMovementDirection::Left))
	{
		return EMovementDirection::Right;
	}
	else if(AngleInRange(Angle, BL_Threshold, FL_Threshold, Buffer, Current != EMovementDirection::Right || Current != EMovementDirection::Left))
	{
		return EMovementDirection::Left;
	}
	else
	{
		return EMovementDirection::Backward;
	}
}

bool UBattleSkyAnimInstance::AngleInRange(const float Angle, const float MinAngle, const float MaxAngle, const float Buffer, const bool IncreaseBuffer) const
{
	bool Result;
	if (IncreaseBuffer)
	{
		Result = Angle >= MinAngle - Buffer && Angle <= MaxAngle + Buffer;
	}
	else
	{
		Result = Angle >= MinAngle + Buffer && Angle <= MaxAngle - Buffer;
	}
	return Result;
}

bool UBattleSkyAnimInstance::CanTurnInPlace() const
{
	return ViewMode == EViewMode::ThirdPerson && RotationMode == ERotationMode::LookingDirection && GetCurveValue(NAME_Enable_Transition) > 0.99f;
}

bool UBattleSkyAnimInstance::CanDynamicTransition() const
{
	return GetCurveValue(NAME_Enable_Transition) == 1.f;
}

void UBattleSkyAnimInstance::RotateInPlaceCheck()
{
	Rotate_L = AimingAngle.X < RotateMinThreshold;
	Rotate_R = AimingAngle.X > RotateMaxThreshold;

	if(Rotate_L || Rotate_R)
	{
		RotateRate = FMath::GetMappedRangeValueClamped(FVector2D(AimYawRateMinRange, AimYawRateMaxRange), FVector2D(MinPlayRate, MaxPlayRate), AimYawRate);
	}
}

void UBattleSkyAnimInstance::TurnInPlaceCheck()
{
	const float AimingDiffAmount = FMath::Abs(AimingAngle.X);
	if (AimingDiffAmount > TurnCheckMinAngle && AimYawRate < AimYawRateLimit)
	{
		ElapsedDelayTime += Delta;
		const float RequiredTurnDelay = FMath::GetMappedRangeValueClamped(FVector2D(TurnCheckMinAngle, 180.f), FVector2D(MinAngleDelay, MaxAngleDelay), AimingDiffAmount);
		if(ElapsedDelayTime > RequiredTurnDelay)
		{
			//TurnInPlace(FRotator(0.f, AimingRotation.Yaw, 0.f), 1.f, 0.f, false);
		}
	}
	else
	{
		ElapsedDelayTime = 0.f;
	}
	return;
}

void UBattleSkyAnimInstance::TurnInPlace(const FRotator TargetRotation, const float PlayRateScale, const float StartTime, const bool OverrideCurrent, bool bRotated90)
{
	const float TurnAngle = FMath::FindDeltaAngleDegrees(OwningCharacter->GetActorRotation().Yaw, TargetRotation.Yaw);
	FTurnInPlace* TargetTurnInPlacePtr = nullptr;
	if (bRotated90)
	{
		if(TurnAngle < 0.f)
		{
			TargetTurnInPlacePtr = Stance == EStance::Standing ? &N_TurnInPlace_L_90 : &CLF_TurnInPlace_L_90;
		}
		else
		{
			TargetTurnInPlacePtr = Stance == EStance::Standing ? &N_TurnInPlace_R_90 : &CLF_TurnInPlace_R_90;
		}
	}
	else
	{
		if (TurnAngle < 0.f)
		{
			TargetTurnInPlacePtr = Stance == EStance::Standing ? &N_TurnInPlace_L_180 : &CLF_TurnInPlace_L_180;
		}
		else
		{
			TargetTurnInPlacePtr = Stance == EStance::Standing ? &N_TurnInPlace_R_180 : &CLF_TurnInPlace_R_180;
		}
	}
	if (!TargetTurnInPlacePtr->Animation)
	{
		return;
	}

	if (OverrideCurrent || !IsPlayingSlotAnimation(TargetTurnInPlacePtr->Animation, TargetTurnInPlacePtr->SlotName))
	{
		PlaySlotAnimationAsDynamicMontage(
			TargetTurnInPlacePtr->Animation,
			TargetTurnInPlacePtr->SlotName,
			0.2f,
			0.2f,
			TargetTurnInPlacePtr->PlayRate * PlayRateScale,
			1,
			0.f,
			StartTime
		);

		if(TargetTurnInPlacePtr->ScaleTurnAngle)
		{
			RotationScale = (TurnAngle / TargetTurnInPlacePtr->AnimatedAngle) * TargetTurnInPlacePtr->PlayRate * PlayRateScale;
		}
		else
		{
			RotationScale = TargetTurnInPlacePtr->PlayRate * PlayRateScale;
		}
	}
}

void UBattleSkyAnimInstance::OnJumped()
{
	Jumped = true;
	JumpPlayRate = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 600.f), FVector2D(1.2f, 1.5f), Speed);
	GetWorld()->GetTimerManager().SetTimer(
		JumpTimerHandle, 
		this,
		&UBattleSkyAnimInstance::ResetJumped,
		0.1f,
		false
	);
}

void UBattleSkyAnimInstance::DynamicTransitionCheck()
{
	USkeletalMeshComponent* Mesh = GetOwningComponent();
	if (Mesh)
	{
		const FVector IK_L_FootLocation = Mesh->GetBoneTransform(NAME_IK_Foot_L, ERelativeTransformSpace::RTS_Component).GetLocation();
		const FVector VB_L_FootLocation = Mesh->GetBoneTransform(NAME_VB_Foot_Target_L, ERelativeTransformSpace::RTS_Component).GetLocation();
		if (FVector::Distance(IK_L_FootLocation, VB_L_FootLocation) > 8.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Dynamic Transition R"));
			PlayDynamicTransition(0.1f, DynamicTransition_R);
		}

		const FVector IK_R_FootLocation = Mesh->GetBoneTransform(NAME_IK_Foot_R, ERelativeTransformSpace::RTS_Component).GetLocation();
		const FVector VB_R_FootLocation = Mesh->GetBoneTransform(NAME_VB_Foot_Target_R, ERelativeTransformSpace::RTS_Component).GetLocation();
		if (FVector::Distance(IK_R_FootLocation, VB_R_FootLocation) > 8.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Dynamic Transition L"));
			PlayDynamicTransition(0.1f, DynamicTransition_L);
		}
	}
}

void UBattleSkyAnimInstance::UpdateInAirValues()
{
	FallSpeed = Velocity.Z;
	LandPrediction = CalculateLandPrediction();
	LeanAmount = LeanAmount.Interp(CalculateInAirLeanAmount(), InAirLeanInterpSpeed, Delta);
}

float UBattleSkyAnimInstance::CalculateLandPrediction()
{
	if (FallSpeed >= -200.f)
	{
		return 0.f;
	}
	if (UCapsuleComponent* Capsule = OwningCharacter->GetCapsuleComponent())
	{
		const FVector CapsuleLocation = Capsule->GetComponentLocation();
		const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
		const float CapsuleHeight = Capsule->GetScaledCapsuleHalfHeight();

		FVector UnSafeNoraml = FVector(Velocity.X, Velocity.Y, FMath::Clamp(Velocity.Z, -4000.f, -200.f)).GetUnsafeNormal();
		UnSafeNoraml *= FMath::GetMappedRangeValueClamped(FVector2D(0.f, -4000.f), FVector2D(50.f, 2000.f), Velocity.Z);

		const FVector TraceEndLocation = CapsuleLocation + UnSafeNoraml;

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwningCharacter);
		
		GetWorld()->SweepSingleByProfile(
			HitResult,
			CapsuleLocation,
			TraceEndLocation,
			FQuat::Identity,
			TEXT("CharacterMesh"),
			FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHeight),
			QueryParams
		);

		if (UCharacterMovementComponent* Mov = OwningCharacter->GetCharacterMovement())
		{
			if (!(Mov->IsWalkable(HitResult) && HitResult.bBlockingHit))
			{
				UE_LOG(LogTemp, Warning, TEXT("In Air Capsule Trace's HitResult is False OR cannot be Walkable"));
				return 0.f;
			}

			UE_LOG(LogTemp, Warning, TEXT("Successfully Calculate LandPrediction Value"));
			return FMath::Lerp(LandPredictionCurve->GetFloatValue(HitResult.Time), 0.f, GetCurveValue(FName("Mask_LandPrediction")));
		}

		UE_LOG(LogTemp, Warning, TEXT("Can't Find CharacterMovementComponent in AnimInstance's OwningCharacter"));
		return 0.f;
	}

	UE_LOG(LogTemp, Warning, TEXT("Can't Find CapsuleComponent in AnimInstance's OwningCharacter"));
	return 0.f;
}

FLeanAmount UBattleSkyAnimInstance::CalculateInAirLeanAmount()
{
	FVector Temp = OwningCharacter->GetActorRotation().UnrotateVector(Velocity) / 350.f;
	Temp *= LeanInAirCurve ? LeanInAirCurve->GetFloatValue(FallSpeed) : 1.f;
	
	return FLeanAmount(Temp.X, Temp.Y);
}
