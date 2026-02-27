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
	
	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void DoSprint(const FInputActionValue& Value);
	void DoFreeLook(const FInputActionValue& Value);
	void DoProne(const FInputActionValue& Value);
	void DoFire(const FInputActionValue& Value);

	virtual FTransform Get3pPivotTarget() const override;
	virtual FVector GetFPCameraTarget() const override;
	virtual void GetCameraParameters(float& OutTP_FOV, float& OutFP_FOV, bool& OutRightShoulder) const override;

	// State Values
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EMovementState MovementState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EMovementState PrevMovementState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EMovementAction MovementAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	ERotationMode RotationMode;
	UPROPERTY(ReplicatedUsing = OnRep_Gait, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EGait Gait;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EStance Stance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
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


private:

	UFUNCTION(Server, Reliable)
	void Server_SetDesiredGait(EGait NewGait);

	UFUNCTION()
	void OnRep_Gait();

	void SetEssentialValues();

	void UpdateCharacterMovement();
	
	EGait GetAllowedGait();
	bool CanSprint() const;
	EGait GetActualGait(const EGait AllowedGait) const;
	void UpdateDynamicMovementSettings(const EGait AllowedGiat);
	FMovementSettings GetTargetMovementSettings() const;
	float GetMappedSpeed() const;

	void SetGait(const EGait NewGait);
	void OnGaitChanged(const EGait NewGait);

	void UpdateGroundedRotation();
	bool CanUpdateMovingRotation();
	void SmoothCharacterRotation(const FRotator Target, const float TargetInterpSpeed, const float ActorInterpSpeed);
	float CalculateGroundedRotationRate() const;
	float GetAnimCurveValue(const FName CurveName) const;

	void LimitRotation(const float AimYawMin, const float AimYawMax, const float InterpSpeed);

	// Referebces
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UAnimInstance* AnimInstance;

	// Essential information
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float MovementInputAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	bool HasMovementInput;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	bool IsMoving;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	FRotator LastVelocityRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float AimYawRate;

	// Cached Values
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cached Values", meta = (AllowPrivateAccess = "true"))
	float PreviousAimYaw;

	// Movement System
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement System", meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle MovementModel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement System", meta = (AllowPrivateAccess = "true"))
	FMovementSettingsState MovementData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement System", meta = (AllowPrivateAccess = "true"))
	FMovementSettings CurrentMovementSettings;

	// Rotation System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement System", meta = (AllowPrivateAccess = "true"))
	FRotator TargetRotation;

	// Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	ERotationMode DesiredRotationMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	EGait DesiredGait;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	EStance DesiredStance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float LookUpDownRate = 1.25f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float LookLeftRightRate = 1.25f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	bool SprintHeld;


protected:	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

	
};

