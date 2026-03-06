// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSkyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "BattleSkyCharacter.h"
#include "EnhancedInputComponent.h"
#include "BattleSkyCameraManager.h"

void ABattleSkyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
		if (!EIC) return;

		
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnMove);
		EIC->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnMouseLook);	

		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnJump);	
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnJumpEnd);
		
		// Toggle
		EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnCrouch);	
		EIC->BindAction(ProneAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnProne);

		EIC->BindAction(ViewModeAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnViewModeChanged);
		EIC->BindAction(WeapongChangeAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnWeaponChange);

		// maintain
		EIC->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnWalk);
		EIC->BindAction(WalkAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnWalk);

		EIC->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnSprint);
		EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnSprint);

		EIC->BindAction(FreeLookAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnFreeLook);
		EIC->BindAction(FreeLookAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnFreeLook);

		EIC->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnFire);
		EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnStopFire);

		EIC->BindAction(AimingAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnAiming);
		EIC->BindAction(AimingAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnAiming);

		EIC->BindAction(PeekingAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnPeeking);
		EIC->BindAction(PeekingAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnPeeking);
		
	}
}

// 호스트가 아닌 클라이언트의 경우 해당 함수는 서버에서만 호출되고,
// 카메라 매니저는 Replicate 대상이 아니므로
// 로컬 카메라 매니저는 제대로 소유한 폰 참조를 얻을 수 없다
// //void ABattleSkyPlayerController::OnPossess(APawn* InPawn)
//{
//	Super::OnPossess(InPawn);
//	if (ABattleSkyCameraManager* BSCameraManager = Cast<ABattleSkyCameraManager>(PlayerCameraManager))
//	{
//		BSCameraManager->OnPossess(InPawn);
//	}
//}

void ABattleSkyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void ABattleSkyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bReturningFromFreeLook)
	{
		ReturnToFreeLookStartRotationByTime(DeltaTime);
	}
}

void ABattleSkyPlayerController::ReturnToFreeLookStartRotationBySpeed(float DeltaTime)
{
	FRotator Current = GetControlRotation();

	float MaxStep = FreeLookReturnInterpSpeed * DeltaTime;

	Current.Yaw = FMath::FixedTurn(
		Current.Yaw,
		FreeLookReturnTargetRotation.Yaw,
		MaxStep
	);

	SetControlRotation(Current);
	if (FMath::IsNearlyEqual(Current.Yaw, FreeLookReturnTargetRotation.Yaw, 0.1f))
	{
		bReturningFromFreeLook = false;
	}
}

void ABattleSkyPlayerController::ReturnToFreeLookStartRotationByTime(float DeltaTime)
{
	FreeLookReturnElapsed += DeltaTime;

	float Alpha = FMath::Clamp(
		FreeLookReturnElapsed / FreeLookReturnDuration,
		0.f,
		1.f
	);
	Alpha = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.f);

	FQuat Start = FreeLookReturnStartRotation.Quaternion();
	FQuat Target = FreeLookReturnTargetRotation.Quaternion();

	FQuat Result = FQuat::Slerp(Start, Target, Alpha);

	SetControlRotation(Result.Rotator());

	if (Alpha >= 1.f)
	{
		bReturningFromFreeLook = false;
	}
}

void ABattleSkyPlayerController::OnMove(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>()) 
	{
		BSCharacter->DoMove(Value.Get<FVector2D>(), FRotator(0.f, IsFreeLooking ? FreeLookReturnTargetRotation.Yaw : ControlRotation.Yaw, 0.f));
	}
}

void ABattleSkyPlayerController::OnMouseLook(const FInputActionValue& Value)
{
	if (bReturningFromFreeLook)
	{
		return;
	}
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ABattleSkyPlayerController::OnJump(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoJump(Value);
	}
}

void ABattleSkyPlayerController::OnJumpEnd(const FInputActionValue& Value)
{
}

void ABattleSkyPlayerController::OnCrouch(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoCrouch(Value);
	}
}

void ABattleSkyPlayerController::OnProne(const FInputActionValue& Value)
{
}

void ABattleSkyPlayerController::OnWalk(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoWalk(Value);
	}
}

void ABattleSkyPlayerController::OnSprint(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoSprint(Value);
	}
}

void ABattleSkyPlayerController::OnFreeLook(const FInputActionValue& Value)
{
	IsFreeLooking = Value.Get<bool>();
	if (IsFreeLooking)
	{
		FreeLookReturnTargetRotation = ControlRotation;
	}
	else
	{
		bReturningFromFreeLook = true;
		FreeLookReturnStartRotation = GetControlRotation();
		FreeLookReturnElapsed = 0.f;

	}
}

void ABattleSkyPlayerController::OnFire(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		const FVector2D ShootReaction = []()->FVector2D 
			{
				return FVector2D(
					FMath::FRandRange(-.5f, -1.f),
					(FMath::RandBool() ? 1.f : -1.f) * FMath::FRandRange(.5f, 1.f)
				);
			}();
		AddPitchInput(ShootReaction.X);
		AddYawInput(ShootReaction.Y);
	}
}

void ABattleSkyPlayerController::OnStopFire(const FInputActionValue& Value)
{	
}

// 시점 변경 시
void ABattleSkyPlayerController::OnViewModeChanged(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->ChangeViewMode(Value);
	}
}

void ABattleSkyPlayerController::OnAiming(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->OverlayState = Value.Get<bool>() ? EOverlayState::Rifle : EOverlayState::Default;
		BSCharacter->RotationMode = Value.Get<bool>() ? ERotationMode::Aiming : ERotationMode::LookingDirection;
	}
}

void ABattleSkyPlayerController::OnPeeking(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoPeeking(Value.Get<float>());
	}
}

void ABattleSkyPlayerController::OnWeaponChange(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoChangeWeapon((int)Value.Get<float>());
	}

}