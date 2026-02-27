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

		// maintain
		EIC->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnWalk);
		EIC->BindAction(WalkAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnWalk);

		EIC->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnSprint);
		EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnSprint);

		EIC->BindAction(FreeLookAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnFreeLook);
		EIC->BindAction(FreeLookAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnFreeLook);

		EIC->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnFire);
		EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnStopFire);
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
}

void ABattleSkyPlayerController::OnMove(const FInputActionValue& Value)
{
	
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>()) 
	{
		BSCharacter->DoMove(Value);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Character"));
	}
}

void ABattleSkyPlayerController::OnMouseLook(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// add yaw and pitch input to controller
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ABattleSkyPlayerController::OnJump(const FInputActionValue& Value)
{
}

void ABattleSkyPlayerController::OnJumpEnd(const FInputActionValue& Value)
{
}

void ABattleSkyPlayerController::OnCrouch(const FInputActionValue& Value)
{

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
}

void ABattleSkyPlayerController::OnFire(const FInputActionValue& Value)
{
}

void ABattleSkyPlayerController::OnStopFire(const FInputActionValue& Value)
{
}
