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
#include "BattleSkyAnimInstance.h"

#include "UIManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

static const FName NAME_Socket_FP_Camera("FP_Camera");


void ABattleSkyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleSkyCharacter, Stance);
	DOREPLIFETIME(ABattleSkyCharacter, Gait);
	DOREPLIFETIME(ABattleSkyCharacter, MovementState);
	DOREPLIFETIME(ABattleSkyCharacter, Replicated_AimingRotation);
	DOREPLIFETIME(ABattleSkyCharacter, Replicated_MovementDirection);
}

ABattleSkyCharacter::ABattleSkyCharacter()
{	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

void ABattleSkyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UUIManagerSubsystem* UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UI->ShowInGameUI(Cast<APlayerController>(GetController()));
	}

	AnimInstance = GetMesh()->GetAnimInstance();
	if (const FMovementSettingsState* P_MovementData = MovementModel.DataTable->FindRow<FMovementSettingsState>(MovementModel.RowName, TEXT("Get Movement Settings")))
	{
		MovementData = *P_MovementData;
	}
	CurrentMovementSettings = MovementData.LookingDriection.Standing;
	UpdateDynamicMovementSettings(Gait);
}

void ABattleSkyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetEssentialValues(DeltaTime);
	if(MovementState == EMovementState::Grounded)
	{
		UpdateGroundedRotation(DeltaTime);
	}
}

void ABattleSkyCharacter::SetEssentialValues(float DeltaTime)
{
	FVector CurrentVelocity = GetVelocity();
	CurrentVelocity.Z = 0.f;
	Speed = CurrentVelocity.Size();
	IsMoving = Speed > 1.f;
	if (IsMoving)
	{
		LastVelocityRotation = GetVelocity().Rotation();
	}

	if (UCharacterMovementComponent* CharMove = GetCharacterMovement())
	{
		MovementInputAmount = CharMove->GetCurrentAcceleration().Length() / CharMove->GetMaxAcceleration();
		HasMovementInput = MovementInputAmount > 0.f;
	}

	// 로컬일 때 컨트롤 회전 서버에 반영
	if (IsLocallyControlled())
	{
		Server_SetAimingRotation(GetControlRotation());

		ActorToAimingRotationDelta = GetControlRotation() - GetActorRotation();
		ActorToAimingRotationDelta.Normalize();
		
		VelocityToAimingRotationDelta = GetVelocity().Rotation() - (IsFreeLooking ? FreeLookStartRotation : GetControlRotation());
		VelocityToAimingRotationDelta.Normalize();

		AimYawRate = FMath::Abs((GetControlRotation().Yaw - PreviousAimYaw) / DeltaTime);
		PreviousAimYaw = GetControlRotation().Yaw;

		Replicated_MovementDirection = CalculateMovementDirection();
	}
	else if (HasAuthority())
	{
		ActorToAimingRotationDelta = Replicated_AimingRotation - GetActorRotation();
		ActorToAimingRotationDelta.Normalize();
		
		VelocityToAimingRotationDelta = GetVelocity().Rotation() - Replicated_AimingRotation;
		VelocityToAimingRotationDelta.Normalize();

		AimYawRate = FMath::Abs((Replicated_AimingRotation.Yaw - PreviousAimYaw) / DeltaTime);
		PreviousAimYaw = Replicated_AimingRotation.Yaw;

		Replicated_MovementDirection = CalculateMovementDirection();
	}
	
}

void ABattleSkyCharacter::UpdateCharacterMovement()
{
	const EGait AllowedGait = GetAllowedGait();
	// Actual Gait 을 구할 필요가 있는지? 배그에서
	/*const EGait ActualGait = GetActualGait(AllowedGait);*/
	if (AllowedGait != Gait)
	{
		SetGait(AllowedGait);
	}
	UpdateDynamicMovementSettings(Gait);
}

void ABattleSkyCharacter::UpdateGroundedRotation(float DeltaTime)
{
	if (IsFreeLooking)
	{
		return;
	}
	if (CanUpdateMovingRotation())
	{
		bTurning = false;

		if(RotationMode == ERotationMode::LookingDirection)
		{
			if (Gait == EGait::Sprinting)
			{
				SmoothCharacterRotation(FRotator(0.f, LastVelocityRotation.Yaw, 0.f), 500.f, CalculateGroundedRotationRate(), DeltaTime);
			}
			else
			{
				SmoothCharacterRotation(FRotator(0.f, Replicated_AimingRotation.Yaw + CalculateYawOffset(), 0.f), 500.f, CalculateGroundedRotationRate(), DeltaTime);
			}
		}
		else
		{
			SmoothCharacterRotation(FRotator(0.f, Replicated_AimingRotation.Yaw, 0.f), 1000.f, 20.f, DeltaTime);
		}
	}
	// 움직이고 있지 않을 때,
	else
	{		
		// 1인칭 혹은 조준 중일 때
		if (CanRotateInPlace())
		{
			SmoothCharacterRotation(FRotator(0.f, Replicated_AimingRotation.Yaw, 0.f), 1000.f, 50.f, DeltaTime);
		}
		// 3인칭 상황에서,
		if (CanTurnInPlace())
		{
			// 액터 회전과 컨트롤 회전 차이가 임계값 이상일 경우 Turn In Place 수행
			TurnInPlaceCheck(DeltaTime);
		}
		else
		{
			ElapsedDelayTime = 0.f;
		}
		if (bTurning)
		{
			SetActorRotatoinDuringTurnInPlace(DeltaTime);
		}
	}
}

// 이동 중이면서, 이동 입력값이 있거나, 속도가 150 이상 이면서 루트모션을 실행 중이 아니라면 MovingRotation 을 업데이트 할 수 있다
bool ABattleSkyCharacter::CanUpdateMovingRotation()
{
	return ((IsMoving && HasMovementInput) || Speed > 150.f) && !HasAnyRootMotion();
}
// 액터의 회전을 적용한다
void ABattleSkyCharacter::SmoothCharacterRotation(const FRotator Target, const float TargetInterpSpeed, const float ActorInterpSpeed, float DeltaTime)
{
	TargetRotation = FMath::RInterpTo(TargetRotation, Target, DeltaTime, TargetInterpSpeed);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, ActorInterpSpeed));
}
// 액터 회전 보간값 반환
float ABattleSkyCharacter::CalculateGroundedRotationRate() const
{
	if (UCurveFloat* RotationCurve = CurrentMovementSettings.RotationRateCurve)
	{
		return RotationCurve->GetFloatValue(GetMappedSpeed()) * FMath::GetMappedRangeValueClamped(FVector2D(0.f, 300.f), FVector2D(1.f, 3.f), AimYawRate);
	}
	return 0.f;
}

float ABattleSkyCharacter::CalculateYawOffset() const
{
	if (!YawOffset_FB || !YawOffset_LR)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character doesn't have Yaw Offset Curve"));
		return 0.f;
	}
	const float RotationDeltaYaw = FMath::FindDeltaAngleDegrees(Replicated_AimingRotation.Yaw, GetVelocity().Rotation().Yaw);

	switch (Replicated_MovementDirection)
	{
	case EMovementDirection::Forward:
		return YawOffset_FB->GetVectorValue(RotationDeltaYaw).X;
	case EMovementDirection::Left:
		return YawOffset_LR->GetVectorValue(RotationDeltaYaw).X;
	case EMovementDirection::Right:
		return YawOffset_LR->GetVectorValue(RotationDeltaYaw).Y;
	case EMovementDirection::Backward:
		return YawOffset_FB->GetVectorValue(RotationDeltaYaw).Y;
	default:
		return 0.f;
	}
}

EMovementDirection ABattleSkyCharacter::CalculateMovementDirection() const
{
	if (Gait == EGait::Sprinting)
	{
		return EMovementDirection::Forward;
	}
	else
	{
		return CalculateQuadrant(
			Replicated_MovementDirection,
			70.f,
			-70.f,
			110.f,
			-110.f,
			5.f,
			VelocityToAimingRotationDelta.Yaw
		);
	}
}

EMovementDirection ABattleSkyCharacter::CalculateQuadrant(
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
	else if (AngleInRange(Angle, BL_Threshold, FL_Threshold, Buffer, Current != EMovementDirection::Right || Current != EMovementDirection::Left))
	{
		return EMovementDirection::Left;
	}
	else
	{
		return EMovementDirection::Backward;
	}
}

bool ABattleSkyCharacter::AngleInRange(const float Angle, const float MinAngle, const float MaxAngle, const float Buffer, const bool IncreaseBuffer) const
{
	if (IncreaseBuffer)
	{
		return Angle >= MinAngle - Buffer && Angle <= MaxAngle + Buffer;
	}
	else
	{
		return Angle >= MinAngle + Buffer && Angle <= MaxAngle - Buffer;
	}
}

void ABattleSkyCharacter::TurnInPlaceCheck(float DeltaTime)
{
	const float AimingDiffAmount = FMath::Abs(ActorToAimingRotationDelta.Yaw);
	if (AimingDiffAmount > TurnCheckMinAngle && AimYawRate < AimYawRateLimit)
	{
		ElapsedDelayTime += DeltaTime;
		const float RequiredTurnDelay = FMath::GetMappedRangeValueClamped(FVector2D(TurnCheckMinAngle, 180.f), FVector2D(MinAngleDelay, MaxAngleDelay), AimingDiffAmount);
		if (ElapsedDelayTime > RequiredTurnDelay && !bTurning)
		{
			const bool bRotated90 = FMath::Abs(ActorToAimingRotationDelta.Yaw) < Turn180Threshold;
			Multicast_PlayTurnInPlace(FRotator(0.f, Replicated_AimingRotation.Yaw, 0.f), bRotated90);
			CurrentTurnInPlace = FTurnInPlaceData(GetActorRotation().Yaw, Replicated_AimingRotation.Yaw, AimingDiffAmount / 150.f);
			bTurning = true;
		}
	}
	else
	{
		ElapsedDelayTime = 0.f;
	}
	return;
}

void ABattleSkyCharacter::SetActorRotatoinDuringTurnInPlace(float DeltaTime)
{
	CurrentTurnInPlace.Elapsed += DeltaTime;

	const float Alpha = FMath::Clamp(CurrentTurnInPlace.Elapsed / CurrentTurnInPlace.Duration, 0.f, 1.f);
	const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentTurnInPlace.StartYaw, CurrentTurnInPlace.TargetYaw);
	float TargetRotationYaw = CurrentTurnInPlace.StartYaw + DeltaYaw * Alpha;

	SetActorRotation(FRotator(0.f, TargetRotationYaw, 0.f));
	if (Alpha >= 1.f)
	{
		bTurning = false;
	}
}

void ABattleSkyCharacter::Multicast_PlayTurnInPlace_Implementation(const FRotator ActorTargetRotation, const bool bRotated90)
{
	if (!AnimInstance)
	{
		const ENetMode NetMode = GetNetMode();

		FString NetModeString;

		switch (NetMode)
		{
		case NM_Client:
			NetModeString = TEXT("Client");
			break;
		case NM_ListenServer:
			NetModeString = TEXT("ListenServer");
			break;
		case NM_DedicatedServer:
			NetModeString = TEXT("DedicatedServer");
			break;
		default:
			NetModeString = TEXT("Standalone");
			break;
		}

		UE_LOG(LogTemp, Warning,
			TEXT("[TurnInPlace] AnimInstance is NULL | Character: %s | NetMode: %s"),
			*GetName(),
			*NetModeString
		);

		return;
	}
	
	if (UBattleSkyAnimInstance* BattleSkyAnim = Cast<UBattleSkyAnimInstance>(AnimInstance))
	{
		BattleSkyAnim->TurnInPlace(ActorTargetRotation, 1.f, 0.f, false, bRotated90);
	}
}

void ABattleSkyCharacter::Server_SetAimingRotation_Implementation(const FRotator NewAimingRotation)
{
	Replicated_AimingRotation = NewAimingRotation;
}

EGait ABattleSkyCharacter::GetAllowedGait()
{
	EGait Result = EGait::Running;
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
		return FMath::Abs(FMath::FindDeltaAngleDegrees(
			IsLocallyControlled() ? GetControlRotation().Yaw : Replicated_AimingRotation.Yaw, 
			CharMove->GetCurrentAcceleration().Rotation().Yaw)) < 50.f && MovementInputAmount > 0.9f;
	}
	return false;
}

//EGait ABattleSkyCharacter::GetActualGait(const EGait AllowedGait) const
//{
//	const float LocalWalkSpeed = CurrentMovementSettings.WalkSpeed;
//	const float LocalRunSpeed = CurrentMovementSettings.RunSpeed;
//	const float LocalSprintSpeed = CurrentMovementSettings.SprintSpeed;
//
//	if (Speed >= LocalRunSpeed + 10.f)
//	{
//		return AllowedGait == EGait::Sprinting ? EGait::Sprinting : EGait::Running;
//	}
//	return Speed >= LocalWalkSpeed + 10.f ? EGait::Running : EGait::Walking;
//}

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

// 현재 회전 모드, 자세에 따른 이동 속도 세팅값을 가져오는 함수 (서버, 클라)
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
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator YawRotation(0, IsFreeLooking ? FreeLookStartRotation.Yaw : Controller->GetControlRotation().Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

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

void ABattleSkyCharacter::DoJump(const FInputActionValue& Value)
{
	Jump();
}

// 서버에서 호출됨
void ABattleSkyCharacter::Server_SetDesiredGait_Implementation(EGait NewGait)
{
	DesiredGait = NewGait;
	UpdateCharacterMovement();
}

void ABattleSkyCharacter::Server_SetDesiredStance_Implementation(EStance NewStance)
{
	DesiredStance = NewStance;
	// Stance 변경이 가능한지 서버에서 판단
	if (DesiredStance == EStance::Crouching && !bIsCrouched)
	{
		Crouch();
	}
	else if (DesiredStance == EStance::Standing && bIsCrouched)
	{
		UnCrouch();
	}
	Stance = DesiredStance;
	UpdateDynamicMovementSettings(Gait);
}

// 서버의 Gait 값이 바뀌었을 때 클라에서 호출됨
void ABattleSkyCharacter::OnRep_Gait()
{
	UpdateDynamicMovementSettings(Gait);
}

// 서버에서 Stance 값이 바뀌었을 때 클라에서 호출됨
void ABattleSkyCharacter::OnRep_Stance()
{
	UpdateDynamicMovementSettings(Gait);
}

// 일단 로컬 변화만 확인
void ABattleSkyCharacter::DoCrouch(const FInputActionValue& Value)
{
	DesiredStance = Stance == EStance::Crouching ? EStance::Standing : EStance::Crouching;
	DesiredStance == EStance::Crouching ? Crouch() : UnCrouch();
	Server_SetDesiredStance(DesiredStance);
}


// 로컬에서만 호출됨
void ABattleSkyCharacter::DoSprint(const FInputActionValue& Value)
{	
	const bool bPressed = Value.Get<bool>();
	EGait NewGait = bPressed ? EGait::Sprinting : EGait::Running;
	if (DesiredGait != NewGait)
	{
		DesiredGait = NewGait;
		//UpdateDynamicMovementSettings(DesiredGait);
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Green,
				FString::Printf(TEXT("DoSprint is Called"))
			);
		}*/
		Server_SetDesiredGait(DesiredGait);
	}
}

void ABattleSkyCharacter::DoFreeLook(const FInputActionValue& Value)
{
	// UE_LOG(LogTemp, Warning, TEXT("Free Look Input Action Value : %s"), Value.Get<bool>() ? TEXT("True") : TEXT("False"));
	const bool bFreeLook = Value.Get<bool>();
	if (bFreeLook)
	{
		IsFreeLooking = bFreeLook;
		FreeLookStartRotation = GetControlRotation();
		OnFreeLookChanged.ExecuteIfBound(bFreeLook, FreeLookStartRotation);
	}
	
}

void ABattleSkyCharacter::ChangeViewMode(const FInputActionValue& Value)
{
	ViewMode = ViewMode == EViewMode::FirstPerson ? EViewMode::ThirdPerson : EViewMode::FirstPerson;
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

void ABattleSkyCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	UE_LOG(LogTemp, Warning, TEXT("OnMovementModeChanged Called"));
	PrevMovementState = MovementState;
	MovementState = ConvertMovementModeToState();
}

// 점프 했을 때 호출
void ABattleSkyCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	InAirRotation = Speed > 100.f ? LastVelocityRotation : GetActorRotation();
	if (UBattleSkyAnimInstance* BSAnim = Cast<UBattleSkyAnimInstance>(AnimInstance))
	{
		BSAnim->OnJumped();
	}
}

EMovementState ABattleSkyCharacter::ConvertMovementModeToState() const
{
	if (UCharacterMovementComponent* Mov = GetCharacterMovement())
	{
		const EMovementMode CurrentMovementMode = Mov->MovementMode;
		if (CurrentMovementMode == EMovementMode::MOVE_NavWalking || CurrentMovementMode == EMovementMode::MOVE_Walking)
		{
			return EMovementState::Grounded;
		}
		else if (CurrentMovementMode == EMovementMode::MOVE_Falling)
		{
			return EMovementState::InAir;
		}
	}
	return EMovementState::Grounded;
}
