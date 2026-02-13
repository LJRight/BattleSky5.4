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


	// Input Action
public:
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
};
