// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CharacterStateTypes.h"
#include "CameraInterface.h"
#include "BattleSkyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ABattleSkyCharacter : public ACharacter, public ICameraInterface
{
	GENERATED_BODY()

public:
	ABattleSkyCharacter();
	

public:
	void DoMove(const FInputActionValue& Value);
	void DoWalk(const FInputActionValue& Value);
	void DoCrouch(const FInputActionValue& Value);
	void DoSprint(const FInputActionValue& Value);
	void DoFreeLook(const FInputActionValue& Value);
	void DoProne(const FInputActionValue& Value);
	void DoFire(const FInputActionValue& Value);

	virtual FTransform Get3pPivotTarget() const override;
	virtual FVector GetFPCameraTarget() const override;
	virtual void GetCameraParameters(float& OutTP_FOV, float& OutFP_FOV, bool& OutRightShoulder) const override;

	// State Values
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EMovementState MovementState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EMovementState PrevMovementState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EMovementAction MovementAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	ERotationMode RotationMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EGait Gait;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EStance Stance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EViewMode ViewMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EOverlayState OverlayState;

	// Camera System
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera System", meta = (AllowPrivateAccess = "true"))
	float ThirdPersonFOV;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera System", meta = (AllowPrivateAccess = "true"))
	float FirstPersonFOV;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera System", meta = (AllowPrivateAccess = "true"))
	bool RightShoulder;



protected:	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

	
};

