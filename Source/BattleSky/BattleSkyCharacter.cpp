// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleSkyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"



DEFINE_LOG_CATEGORY(LogTemplateCharacter);

static const FName NAME_Socket_FP_Camera("FP_Camera");

//////////////////////////////////////////////////////////////////////////
// ABattleSkyCharacter

ABattleSkyCharacter::ABattleSkyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}



void ABattleSkyCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	AnimInstance = GetMesh()->GetAnimInstance();
	if (const FMovementSettingsState* P_MovementData = MovementModel.DataTable->FindRow<FMovementSettingsState>(MovementModel.RowName, TEXT("Get Movement Settings")))
	{
		MovementData = *P_MovementData;
	}
	CurrentMovementSettings = MovementData.LookingDriection.Standing;
	//DesiredGait = EGait::Running;
}

void ABattleSkyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetEssentialValues();
	if(MovementState == EMovementState::Grounded)
	{
		//UpdateCharacterMovement();
		UpdateGroundedRotation();
	}
}



void ABattleSkyCharacter::SetEssentialValues()
{
	FVector CurrentVelocity = GetVelocity();
	CurrentVelocity.Z = 0.f;
	Speed = CurrentVelocity.Size();
	IsMoving = Speed > 1.f;
	if (IsMoving)
	{
		LastVelocityRotation = GetVelocity().Rotation();
	}

	if(UCharacterMovementComponent* CharMove = GetCharacterMovement())
	{
		MovementInputAmount = CharMove->GetCurrentAcceleration().Length() / CharMove->GetMaxAcceleration();
		HasMovementInput = MovementInputAmount > 0.f;
	}

	AimYawRate = FMath::Abs((GetControlRotation().Yaw - PreviousAimYaw) / GetWorld()->GetDeltaSeconds());
	PreviousAimYaw = GetControlRotation().Yaw;
}

void ABattleSkyCharacter::UpdateCharacterMovement()
{
	const EGait AllowedGait = GetAllowedGait();
	const EGait ActualGait = GetActualGait(AllowedGait);
	if (ActualGait != Gait)
	{
		SetGait(ActualGait);
	}
	UpdateDynamicMovementSettings(AllowedGait);
}

void ABattleSkyCharacter::UpdateGroundedRotation()
{
	UE_LOG(LogTemp,Warning, TEXT("In Update Grounded Rotation"))
	if (CanUpdateMovingRotation())
	{
		if(RotationMode == ERotationMode::LookingDirection)
		{
			if (Gait == EGait::Sprinting)
			{
				SmoothCharacterRotation(
					FRotator(0.f, LastVelocityRotation.Yaw, 0.f), 
					500.f, 
					CalculateGroundedRotationRate()
				);
			}
			else
			{
				SmoothCharacterRotation(
					FRotator(0.f, GetControlRotation().Yaw + GetAnimCurveValue(FName("YawOffset")), 0.f),
					500.f,
					CalculateGroundedRotationRate()
				);
			}
		}
		else
		{
			SmoothCharacterRotation(
				FRotator(0.f, GetControlRotation().Yaw, 0.f),
				1000.f, 
				20.f
			);
		}
	}
	// Not Moving
	else
	{
		if (ViewMode == EViewMode::FirstPerson || RotationMode == ERotationMode::Aiming)
		{
			LimitRotation(-100.f, 100.f, 20.f);;
		}
		const float RotationAmount = GetAnimCurveValue(FName("RotationAmount"));
		if (FMath::Abs(RotationAmount) > 0.001f)
		{
			AddActorWorldRotation(FRotator(0.f, RotationAmount * (GetWorld()->GetDeltaSeconds() / (1.f / 30.f)), 0.f));
			TargetRotation = GetActorRotation();
		}
	}
}

bool ABattleSkyCharacter::CanUpdateMovingRotation()
{
	return ((IsMoving && HasMovementInput) || Speed > 150.f) && !HasAnyRootMotion();
}

void ABattleSkyCharacter::SmoothCharacterRotation(const FRotator Target, const float TargetInterpSpeed, const float ActorInterpSpeed)
{
	TargetRotation = FMath::RInterpTo(TargetRotation, Target, GetWorld()->GetDeltaSeconds(), TargetInterpSpeed);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), ActorInterpSpeed));
}

float ABattleSkyCharacter::CalculateGroundedRotationRate() const
{
	if (UCurveFloat* RotationCurve = CurrentMovementSettings.RotationRateCurve)
	{
		return RotationCurve->GetFloatValue(GetMappedSpeed()) * FMath::GetMappedRangeValueClamped(FVector2D(0.f, 300.f), FVector2D(1.f, 3.f), AimYawRate);
	}
	return 0.f;
}

float ABattleSkyCharacter::GetAnimCurveValue(const FName CurveName) const
{	
	return AnimInstance ? AnimInstance->GetCurveValue(CurveName) : 0.f;
}

void ABattleSkyCharacter::LimitRotation(const float AimYawMin, const float AimYawMax, const float InterpSpeed)
{
	const float AimingDiff = (GetControlRotation() - GetActorRotation()).Yaw;
	if (!(AimingDiff >= AimYawMin && AimingDiff <= AimYawMax))
	{
		const float TargetYawOffset = AimingDiff > 0.f ? AimYawMin : AimYawMax;
		SmoothCharacterRotation(
			FRotator(0.f, GetActorRotation().Yaw + TargetYawOffset, 0.f),
			0.f,
			InterpSpeed
		);
	}
}

EGait ABattleSkyCharacter::GetAllowedGait()
{
	EGait Result;
	switch (Stance)
	{
		case EStance::Standing:
			if (RotationMode == ERotationMode::LookingDirection)
			{
				Result = DesiredGait != EGait::Sprinting ? DesiredGait : CanSprint() ? EGait::Sprinting : EGait::Running;
			}
			else
			{
				Result = DesiredGait == EGait::Walking ? EGait::Walking : EGait::Running;
			}
			break;
		case EStance::Crouching:
			Result = DesiredGait == EGait::Walking ? EGait::Walking : EGait::Running;
			break;
		case EStance::Prone:
			Result = EGait::Walking;
			break;
	}
	return Result;
}

bool ABattleSkyCharacter::CanSprint() const
{
	if (!HasMovementInput || RotationMode == ERotationMode::Aiming)
	{
		// 입력이 없거나 조준 모드일 때는 스프린트 불가능
		return false;
	}
	if(UCharacterMovementComponent* CharMove = GetCharacterMovement())
	{
		// 입력이 있으면서 컨트롤 회전(바라보는)과 이동 가속도(움직이고자 하는 방향)의 차이가 50(변수로 조정 가능)보다 작아야 스프린트 가능
		return FMath::Abs(FMath::FindDeltaAngleDegrees(GetControlRotation().Yaw, CharMove->GetCurrentAcceleration().Rotation().Yaw)) < 50.f && MovementInputAmount > 0.9f;
	}
	return false;
}

EGait ABattleSkyCharacter::GetActualGait(const EGait AllowedGait) const
{
	const float LocalWalkSpeed = CurrentMovementSettings.WalkSpeed;
	const float LocalRunSpeed = CurrentMovementSettings.RunSpeed;
	const float LocalSprintSpeed = CurrentMovementSettings.SprintSpeed;

	if (Speed >= LocalRunSpeed + 10.f)
	{
		return AllowedGait == EGait::Sprinting ? EGait::Sprinting : EGait::Running;
	}
	return Speed >= LocalWalkSpeed + 10.f ? EGait::Running : EGait::Walking;
}

void ABattleSkyCharacter::UpdateDynamicMovementSettings(const EGait AllowedGait)
{
	CurrentMovementSettings = GetTargetMovementSettings();
	if(UCharacterMovementComponent* CharMove = GetCharacterMovement())
	{
		float TargetMaxSpeed = AllowedGait == EGait::Sprinting ? CurrentMovementSettings.SprintSpeed : AllowedGait == EGait::Running ? CurrentMovementSettings.RunSpeed : CurrentMovementSettings.WalkSpeed;
		CharMove->MaxWalkSpeed = TargetMaxSpeed;
		CharMove->MaxWalkSpeedCrouched = TargetMaxSpeed;
		if (UCurveVector* MovementCurve = CurrentMovementSettings.MovementCurve)
		{
			const FVector MovementCurveVectorValue = MovementCurve->GetVectorValue(GetMappedSpeed());
			CharMove->MaxAcceleration = MovementCurveVectorValue.X;
			CharMove->BrakingDecelerationWalking = MovementCurveVectorValue.Y;
			CharMove->GroundFriction = MovementCurveVectorValue.Z;
		}
	}
}

FMovementSettings ABattleSkyCharacter::GetTargetMovementSettings() const
{
	FMovementSettingsStance StanceSettings;
	switch (RotationMode)
	{
		case ERotationMode::VelocityDirection:
			StanceSettings = MovementData.VelocityDirection;
			break;
		case ERotationMode::LookingDirection:	
			StanceSettings = MovementData.LookingDriection;
			break;
		case ERotationMode::Aiming:	
			StanceSettings = MovementData.Aiming;
			break;
	}
	switch (Stance)
	{
		case EStance::Standing:
			return StanceSettings.Standing;
		case EStance::Crouching:
			return StanceSettings.Crouhcing;
		case EStance::Prone:
			return StanceSettings.Prone;
		default:
			return FMovementSettings();
	}
}

float ABattleSkyCharacter::GetMappedSpeed() const
{
	const float LocalWalkSpeed = CurrentMovementSettings.WalkSpeed;
	const float LocalRunSpeed = CurrentMovementSettings.RunSpeed;
	const float LocalSprintSpeed = CurrentMovementSettings.SprintSpeed;

	if (Speed > LocalRunSpeed)
	{
		return FMath::GetMappedRangeValueClamped(FVector2D(LocalRunSpeed, LocalSprintSpeed), FVector2D(2.f, 3.f), Speed);
	}
	else if (Speed > LocalWalkSpeed)
	{
		return FMath::GetMappedRangeValueClamped(FVector2D(LocalWalkSpeed, LocalRunSpeed), FVector2D(1.f, 2.f), Speed);
	}
	else
	{
		return FMath::GetMappedRangeValueClamped(FVector2D(0.f, LocalWalkSpeed), FVector2D(0.f, 1.f), Speed);
	}
}

void ABattleSkyCharacter::SetGait(const EGait NewGait)
{
	if(NewGait != Gait)
	{
		OnGaitChanged(NewGait);
	}
}

void ABattleSkyCharacter::OnGaitChanged(const EGait NewGait)
{
	const EGait PreviousGait = Gait; 
	Gait = NewGait;
}

void ABattleSkyCharacter::DoMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABattleSkyCharacter::DoWalk(const FInputActionValue& Value)
{
	const bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		DesiredGait = EGait::Walking;
		Server_SetDesiredGait(EGait::Walking);
	}
	else
	{
		DesiredGait = EGait::Running;
		Server_SetDesiredGait(EGait::Running);
	}
}

void ABattleSkyCharacter::Server_SetDesiredGait_Implementation(EGait NewGait)
{
	DesiredGait = NewGait;

	const EGait AllowedGait = GetAllowedGait();
	const EGait ActualGait = GetActualGait(AllowedGait);

	SetGait(AllowedGait);

	UpdateDynamicMovementSettings(AllowedGait);
}

void ABattleSkyCharacter::OnRep_Gait()
{
	UpdateDynamicMovementSettings(Gait);

}

void ABattleSkyCharacter::DoCrouch(const FInputActionValue& Value)
{
}

void ABattleSkyCharacter::DoSprint(const FInputActionValue& Value)
{
	const bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		DesiredGait = EGait::Sprinting;
		Server_SetDesiredGait(DesiredGait);
	}
	else
	{
		DesiredGait = EGait::Running;
		Server_SetDesiredGait(DesiredGait);
	}
}

FTransform ABattleSkyCharacter::Get3pPivotTarget() const
{
	return GetActorTransform();
}

FVector ABattleSkyCharacter::GetFPCameraTarget() const
{
	return GetMesh()->GetSocketLocation(NAME_Socket_FP_Camera);
}

void ABattleSkyCharacter::GetCameraParameters(float& OutTP_FOV, float& OutFP_FOV, bool& OutRightShoulder) const
{
	OutTP_FOV = ThirdPersonFOV;
	OutFP_FOV = FirstPersonFOV;
	OutRightShoulder = RightShoulder;
}

void ABattleSkyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleSkyCharacter, Stance);
	DOREPLIFETIME(ABattleSkyCharacter, Gait);
	DOREPLIFETIME(ABattleSkyCharacter, MovementState);
}
