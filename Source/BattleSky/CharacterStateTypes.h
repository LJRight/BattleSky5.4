#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "CharacterStateTypes.generated.h"

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
    Forward UMETA(DisplayName = "Forward"),
    Backward UMETA(DisplayName = "Backward"),
    Left UMETA(DisplayName = "Left"),
    Right UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class EGait : uint8
{
    Walking    UMETA(DisplayName = "Walking"),
    Running     UMETA(DisplayName = "Running"),
    Sprinting UMETA(DisplayName = "Sprinting")
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
    None UMETA(DisplayName = "None"),
    Grounded UMETA(DisplayName = "Grounded"),
    InAir UMETA(DisplayName = "In Air"),
    Mantling UMETA(DisplayName = "Mantling"),
	Ragdoll UMETA(DisplayName = "Ragdoll"),
};

UENUM(BlueprintType)
enum class EStance : uint8
{
    Standing UMETA(DisplayName = "Standing"),
    Crouching UMETA(DisplayName = "Crouching"),
    Prone UMETA(DisplayName = "Prone")
};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
    VelocityDirection UMETA(DisplayName = "VelocityDirection"),
    LookingDirection UMETA(DisplayName = "LookDirection"),
    Aiming UMETA(DisplayName = "Aiming"),
};

UENUM(BlueprintType)
enum class EMovementAction : uint8
{
    None UMETA(DisplayName = "None"),
    LowMantle UMETA(DisplayName = "LowMantle"),
    HighMantle UMETA(DisplayName = "HighMantle"),
    Rolling UMETA(DisplayName = "Rolling"),
    GettingUp UMETA(DisplayName = "GettingUp"),
};

UENUM(BlueprintType)
enum class EOverlayState : uint8
{
    Default UMETA(DisplayName = "None"),
    Masculine UMETA(DisplayName = "LowMantle"),
    Feminine UMETA(DisplayName = "HighMantle"),
    Injured UMETA(DisplayName = "Rolling"),
    HandsTied UMETA(DisplayName = "GettingUp"),
    Rifle UMETA(DisplayName = "Rifle"),
    Pistol_1H UMETA(DisplayName = "Pistol 1H"),
    Pistol_2H UMETA(DisplayName = "Pistol 2H"),
    Bow UMETA(DisplayName = "Bow"),
    Torch UMETA(DisplayName = "Torch"),
    Binoculars UMETA(DisplayName = "Binoculars"),
    Box UMETA(DisplayName = "Box"),
    Barrel UMETA(DisplayName = "Barrel"),
};

UENUM(BlueprintType)
enum class EViewMode : uint8
{
	ThirdPerson UMETA(DisplayName = "ThirdPerson"),
	FirstPerson UMETA(DisplayName = "FirstPerson"),
};

UENUM(BlueprintType)
enum class EGroundedEntryState : uint8
{
    None UMETA(DisplayName = "None"),
    Roll UMETA(DisplayName = "Roll"),
};


USTRUCT(BlueprintType)
struct FMovementSettings
{
    GENERATED_BODY()
    FMovementSettings(float WalkSpeed = 0.f, float RunSpeed = 0.f, float SprintSpeed = 0.f, UCurveVector* MovementCurve = nullptr, UCurveFloat* RotationRateCurve = nullptr)
        : WalkSpeed(WalkSpeed), RunSpeed(RunSpeed), SprintSpeed(SprintSpeed), MovementCurve(MovementCurve), RotationRateCurve(RotationRateCurve)
    {
	}
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float WalkSpeed = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float RunSpeed = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SprintSpeed = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UCurveVector* MovementCurve = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UCurveFloat* RotationRateCurve = nullptr;
};

USTRUCT(BlueprintType)
struct FMovementSettingsStance
{
    GENERATED_BODY()
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FMovementSettings Standing;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FMovementSettings Crouhcing;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FMovementSettings Prone;
};

USTRUCT(BlueprintType)
struct FMovementSettingsState : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FMovementSettingsStance VelocityDirection;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FMovementSettingsStance LookingDriection;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FMovementSettingsStance Aiming;
};

USTRUCT(BlueprintType)
struct FMantle_TraceSettings
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MaxLedgeHeight;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MinLedgeHeight;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ReachDistance;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ForwardTraceRadius;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DownwardTraceRadius;
};

USTRUCT(BlueprintType)
struct FMantle_Params
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UAnimMontage* AnimMontage = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UCurveVector* PositionCorrectionCurve = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float StartingPosition;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float PlayRate;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector StartingOffset;
};

USTRUCT(BlueprintType)
struct FComponent_Transform
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FTransform Transform;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UPrimitiveComponent* Component = nullptr;
};

USTRUCT(BlueprintType)
struct FDebugInfo
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly)
    class ACharacter* DebugFocusCharacter = nullptr;
    UPROPERTY(BlueprintReadOnly)
    bool DebugView;
    UPROPERTY(BlueprintReadOnly)
    bool ShowHUD;
    UPROPERTY(BlueprintReadOnly)
    bool ShowTraces;
    UPROPERTY(BlueprintReadOnly)
    bool ShowDebugShapes;
    UPROPERTY(BlueprintReadOnly)
    bool ShowLayerColors;
    UPROPERTY(BlueprintReadOnly)
    bool Slomo;
	UPROPERTY(BlueprintReadOnly)
    bool ShowCharacterInfo;
};
