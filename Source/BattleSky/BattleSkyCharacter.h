// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CharacterStateTypes.h"
#include "CameraInterface.h"
#include "Components/SphereComponent.h"

#include "BattleSkyCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;


class AItemActor;


USTRUCT(BlueprintType)
struct FTurnInPlaceData
{
	GENERATED_BODY()
	FTurnInPlaceData(const float StartYaw = 0.f, const float TargetYaw = 0.f, const float Duration = 0.f)
		: StartYaw(StartYaw), TargetYaw(TargetYaw), Duration(Duration), Elapsed(0.f)
	{
	};
	float StartYaw;
	float TargetYaw;
	float Duration;
	float Elapsed;
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class ABattleSkyCharacter : public ACharacter, public ICameraInterface
{
	GENERATED_BODY()

public:
	ABattleSkyCharacter();

	// ���ø�����Ʈ ���� ���� �Լ�
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ���� �Է¿� ���� ����Ǵ� �Լ�
	void DoMove(const FVector2D MovementVector, const FRotator BaseRotation);
	void DoWalk(const FInputActionValue& Value);
	void DoJump(const FInputActionValue& Value);
	void ChangeViewMode(const FInputActionValue& Value);
	void DoCrouch(const FInputActionValue& Value);
	void DoSprint(const FInputActionValue& Value);

	void DoProne(const FInputActionValue& Value);
	void DoFire(const FVector Start, const FRotator Rotation);

	void DoPeeking(const float PeekingDirection);

	// �ֺ� ��ü Ž�� ����

	void SearchAround(const bool bSearch);

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SearchSphere;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnNearByUpdated, const TArray<AItemActor*>&);
	FOnNearByUpdated OnNearByUpdated;
	UPROPERTY(VisibleAnywhere)
	TArray<AItemActor*> NearbyItems;

	UFUNCTION()
	void OnItemEnter(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnItemLeave(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	// �κ��丮 
	UFUNCTION(Server, Reliable)
	void Server_DoPeeking(const float Value);
	UPROPERTY(Replicated)
	EPeekingDirection Replicated_PeekingDirection = EPeekingDirection::None;

	void DoChangeWeapon(const int WeaponIndex);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnPickupItem();

	UFUNCTION(BlueprintCallable)
	void AttachWeapon();

	// void AttachToBody(AWeaponBase* TargetWeapon, EWeaponSlot TargetWeaponSlot);

	/*UPROPERTY(Replicated)
	AWeaponBase* Replicated_CurrentEquipedWeapon;*/


	//UFUNCTION()
	//void HoldWeapon(AWeaponBase* TargetWeapon);



	// ī�޶� �Ŵ���(����)���� �ʿ��� ������ ���� ȣ���ϴ� �Լ�
	virtual FTransform Get3pPivotTarget() const override;
	virtual FVector GetFPCameraTarget() const override;
	virtual void GetCameraParameters(float& OutTP_FOV, float& OutFP_FOV, bool& OutRightShoulder) const override;

	// ī�޶� �Ŵ������� ��ȯ�ϴ� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera System", meta = (AllowPrivateAccess = "true"))
	float ThirdPersonFOV;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera System", meta = (AllowPrivateAccess = "true"))
	float FirstPersonFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera System", meta = (AllowPrivateAccess = "true"))
	bool RightShoulder;


	// Event Override
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	virtual void OnJumped_Implementation() override;

	EMovementState ConvertMovementModeToState() const;

	// State Values
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EMovementState MovementState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EMovementState PrevMovementState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EMovementAction MovementAction;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	ERotationMode Replicated_RotationMode;
	UPROPERTY(ReplicatedUsing = OnRep_Gait, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EGait Gait;
	UPROPERTY(ReplicatedUsing = OnRep_Stance, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EStance Stance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EViewMode ViewMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "State Values")
	EOverlayState OverlayState;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EMovementDirection Replicated_MovementDirection;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FRotator Replicated_AimingRotation;







	EMovementDirection CalculateMovementDirection() const;
	EMovementDirection CalculateQuadrant(const EMovementDirection Current, const float FR_Threshold, const float FL_Threshold, const float BR_Threshold, const float BL_Threshold, const float Buffer, const float Angle) const;
	bool AngleInRange(const float Angle, const float MinAngle, const float MaxAngle, const float Buffer, const bool IncreaseBuffer) const;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actor Rotation Curves", meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffset_FB;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actor Rotation Curves", meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffset_LR;



	// Turn In place
	FORCEINLINE bool CanRotateInPlace() const { return Replicated_RotationMode == ERotationMode::Aiming || ViewMode == EViewMode::FirstPerson; };
	FORCEINLINE bool CanTurnInPlace() const { return ViewMode == EViewMode::ThirdPerson && Replicated_RotationMode == ERotationMode::LookingDirection; };
	void TurnInPlaceCheck(float DeltaTime);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayTurnInPlace(const FRotator ActorTargetRotation, const bool bRotated90);
	void SetActorRotatoinDuringTurnInPlace(float DeltaTime);

	FTurnInPlaceData CurrentTurnInPlace;
	FRotator ActorToAimingRotationDelta;
	FRotator VelocityToAimingRotationDelta;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float TurnCheckMinAngle = 45.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Turn180Threshold = 130.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float AimYawRateLimit = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float ElapsedDelayTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float MinAngleDelay = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float MaxAngleDelay = 0.f;
	bool bTurning;

private:

	// Server RPC
	UFUNCTION(Server, Reliable)
	void Server_SetDesiredGait(EGait NewGait);
	UFUNCTION(Server, Reliable)
	void Server_SetDesiredStance(EStance NewStance);
	UFUNCTION()
	void OnRep_Gait();
	UFUNCTION()
	void OnRep_Stance();

	void SetEssentialValues(float DeltaTime);
	void UpdateCharacterMovement();

	EGait GetAllowedGait();
	bool CanSprint() const;
	void UpdateDynamicMovementSettings(const EGait AllowedGiat);
	FMovementSettings GetTargetMovementSettings() const;
	float GetMappedSpeed() const;

	void SetGait(const EGait NewGait);
	void OnGaitChanged(const EGait NewGait);

	void UpdateGroundedRotation(float DeltaTime);
	bool CanUpdateMovingRotation();
	void SmoothCharacterRotation(const FRotator Target, const float TargetInterpSpeed, const float ActorInterpSpeed, float DeltaTime);
	float CalculateGroundedRotationRate() const;
	float CalculateYawOffset() const;

	// References
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References", meta = (AllowPrivateAccess = "true"))
	UAnimInstance* AnimInstance;

	// Essential information
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential information", meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential information", meta = (AllowPrivateAccess = "true"))
	float MovementInputAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential information", meta = (AllowPrivateAccess = "true"))
	bool HasMovementInput;

public:
	FORCEINLINE bool Get_HasMovementInput() const { return HasMovementInput; };
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential information", meta = (AllowPrivateAccess = "true"))
	bool IsMoving;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential information", meta = (AllowPrivateAccess = "true"))
	FRotator LastVelocityRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential information", meta = (AllowPrivateAccess = "true"))
	float AimYawRate;




	// Actor Rotation
	UFUNCTION(Server, Unreliable)
	void Server_SetAimingRotation(const FRotator NewAimingRotation);
	void TrySendAimingRotation(float DeltaTime, const FRotator CurrentRotation);
	FRotator GetEffectiveAimingRotation() const;

	float AimingSendElapsed = 0.0f;
	FRotator LastSentRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, Category = "Network Settings", meta = (AllowPrivateAccess = "true"))
	float NormalSendingInterval = 0.05f;
	UPROPERTY(EditAnywhere, Category = "Network Settings", meta = (AllowPrivateAccess = "true"))
	float RotationDegreeDiffThreshold = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Network Settings", meta = (AllowPrivateAccess = "true"))
	float SendingTimerThreshold = 0.2f;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotation System", meta = (AllowPrivateAccess = "true"))
	FRotator TargetRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotation System", meta = (AllowPrivateAccess = "true"))
	FRotator InAirRotation;

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

