// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSkyAnimInstance.h"
#include "BattleSkyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

				// 멈춰 있는 동안
				else
				{

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
	AimingRotation = OwningCharacter->GetControlRotation();
	AimYawRate = (AimingRotation.Yaw - PreviousAimYaw) / Delta;

	if(ABattleSkyCharacter* BattleSkyCharacter = Cast<ABattleSkyCharacter>(OwningCharacter))
	{
		MovementState = BattleSkyCharacter->MovementState;
		PrevMovementState = BattleSkyCharacter->PrevMovementState;
		MovementAction = BattleSkyCharacter->MovementAction;
		Gait = BattleSkyCharacter->Gait;
		Stance = BattleSkyCharacter->Stance;
		OverlayState = BattleSkyCharacter->OverlayState;
	}
}

void UBattleSkyAnimInstance::UpdateAimingValues()
{
	SmoothedAimingRotation = FMath::RInterpTo(SmoothedAimingRotation, AimingRotation, Delta, SmoothedAimingRotationInterpSpeed);

	const FRotator ActorRotation = OwningCharacter->GetActorRotation();

	const FRotator DeltaAimingRotation = AimingRotation - ActorRotation;
	AimingAngle = FVector2D(DeltaAimingRotation.Yaw, DeltaAimingRotation.Pitch);

	const FRotator DeltaSmoothedAimingRotation = SmoothedAimingRotation - ActorRotation;
	SmoothedAimingAngle = FVector2D(DeltaSmoothedAimingRotation.Yaw, DeltaSmoothedAimingRotation.Pitch);

	if(RotationMode != ERotationMode::VelocityDirection)
	{
		AimSweepTime = FMath::GetMappedRangeValueClamped(FVector2D(-90.f, 90.f), FVector2D(1.f, 0.f), AimingAngle.Y);

		// Use the Aiming Yaw Angle divided by the number of spine + pelvis bones to get the amount of 
		// spine rotation needed to remain facing the camera direction
		SpineRotation.Yaw = AimingAngle.X / 4.0f;
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
	return FMath::Clamp(SprintPlayRate / StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.f, 3.f);
}

float UBattleSkyAnimInstance::CalculateCrouchingPlayRate()
{
	FMath::Clamp(Speed / AnimatedCrouchSpeed / StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.f, 2.f);
}

void UBattleSkyAnimInstance::UpdateRotationValues()
{
	

}

//// 현재 프레임과 이전 프레임의 속도 차이를 이용해 가속도를 계산한다
//FVector UBattleSkyAnimInstance::CalculateAcceleration(const FVector& CurrentVelocity, float DeltaTime) const
//{
//	return (CurrentVelocity - PreviousVelocity) / DeltaTime;
//}
