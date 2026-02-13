// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSkyAnimInstance.h"
#include "BattleSkyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

const static FName NAME_Mask_AimOffset("Mask_AimOffset");


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
		}
	}
}

void UBattleSkyAnimInstance::UpdateCharacterInfo()
{
	Velocity = OwningCharacter->GetVelocity();
	Acceleration = CalculateAcceleration(Velocity, Delta);
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
}

void UBattleSkyAnimInstance::UpdateFootIK()
{
}

// 현재 프레임과 이전 프레임의 속도 차이를 이용해 가속도를 계산한다
FVector UBattleSkyAnimInstance::CalculateAcceleration(const FVector& CurrentVelocity, float DeltaTime) const
{
	return (CurrentVelocity - PreviousVelocity) / DeltaTime;
}
