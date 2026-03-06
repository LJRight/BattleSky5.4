// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "BattleSkyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API ABattleSkyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	virtual void SetupInputComponent() override;

	// virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ReturnToFreeLookStartRotationBySpeed(float DeltaTime);
	void ReturnToFreeLookStartRotationByTime(float DeltaTime);

	// Input Action
public:
	
	UPROPERTY(VisibleAnywhere, Category="Free Look")
	bool IsFreeLooking;
	UPROPERTY(VisibleAnywhere, Category = "Free Look")
	bool bReturningFromFreeLook;
	UPROPERTY(VisibleAnywhere, Category = "Free Look")
	FRotator FreeLookReturnTargetRotation;
	
	// 속도 보간시 필요
	FRotator FreeLookReturningCurrentRotation;
	float FreeLookReturnInterpSpeed = 500.f;

	// 시간 기반 보간시 필요
	FRotator FreeLookReturnStartRotation;

	float FreeLookReturnElapsed = 0.f;
	UPROPERTY(VisibleAnywhere, Category = "Free Look")
	float FreeLookReturnDuration = .01f;

	



	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MouseLookAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ProneAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* WalkAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FreeLookAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ViewModeAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AimingAction;


	// Input Action Handlers
	void OnMove(const FInputActionValue& Value);
	void OnMouseLook(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnJumpEnd(const FInputActionValue& Value);
	void OnCrouch(const FInputActionValue& Value);
	void OnProne(const FInputActionValue& Value);
	void OnWalk(const FInputActionValue& Value);
	void OnSprint(const FInputActionValue& Value);
	void OnFreeLook(const FInputActionValue& Value);
	void OnFire(const FInputActionValue& Value);
	void OnStopFire(const FInputActionValue& Value);
	void OnViewModeChanged(const FInputActionValue& Value);
	void OnAiming(const FInputActionValue& Value);

private : 
	bool IsAiming = false;
};
