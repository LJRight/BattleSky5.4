// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSkyCameraManager.h"
#include "BattleSkyAnimInstance.h"
#include "BattleSkyCameraAnimInstance.h"
#include "CameraInterface.h"

#include "BattleSkyCharacter.h"

#include "DrawDebugHelpers.h"

static const FName NAME_RotationLagSpeed("RotationLagSpeed");

static const FName NAME_PivotLagSpeed_X("PivotLagSpeed_X");
static const FName NAME_PivotLagSpeed_Y("PivotLagSpeed_Y");
static const FName NAME_PivotLagSpeed_Z("PivotLagSpeed_Z");

static const FName NAME_PivotOffset_X("PivotOffset_X");
static const FName NAME_PivotOffset_Y("PivotOffset_Y");
static const FName NAME_PivotOffset_Z("PivotOffset_Z");

static const FName NAME_CameraOffset_X("CameraOffset_X");
static const FName NAME_CameraOffset_Y("CameraOffset_Y");
static const FName NAME_CameraOffset_Z("CameraOffset_Z");

static const FName NAME_Weight_FirstPerson("Weight_FirstPerson");

ABattleSkyCameraManager::ABattleSkyCameraManager()
{
	CameraBehavior = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Camera Behavior"));
	CameraBehavior->SetupAttachment(RootComponent);
}

// 리슨 서버에 있는 클라이언트의 경우 로컬 PlayerController 에서 OnPossess 가 호출되지 않는다. 
//void ABattleSkyCameraManager::OnPossess(APawn* NewPawn)
//{
//	ControlledPawn = NewPawn;
//	if (UBattleSkyCameraAnimInstance* BSAnimInstance = Cast<UBattleSkyCameraAnimInstance>(CameraBehavior->GetAnimInstance()))
//	{
//		 BSAnimInstance->SetOwningController(GetOwningPlayerController());
//		 BSAnimInstance->SetControlledPawn(ControlledPawn);
//	}
//}

/* 캐릭터 정보 수집
→ 카메라 회전 계산
→ 피벗 위치 스무딩
→ 실제 피벗 위치 오프셋
→ 카메라 위치 계산
→ 충돌 보정
→ 디버그 시각화
→ 최종 ViewTarget.POV 확정
*/
void ABattleSkyCameraManager::CustomCameraBehavior(FVector& OutLocation, FRotator& OutRotation, float& OutFOV)
{
	// 로컬 카메라 매니저는 매 틱마다 컨트롤러와 소유한 폰을 찾는다
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (!OwningPlayerController)
	{
		return;
	}
	ControlledPawn = OwningPlayerController->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}
	if (UBattleSkyCameraAnimInstance* BSCameraAnimInstance = Cast<UBattleSkyCameraAnimInstance>(CameraBehavior->GetAnimInstance()))
	{
		BSCameraAnimInstance->SetOwningController(GetOwningPlayerController());
		BSCameraAnimInstance->SetControlledPawn(ControlledPawn);
	}
	// Step 1: Get Camera Parameters from CharacterBP via the Camera Interface
	FTransform PivotTarget;
	FVector FPTarget;
	float TPFOV, FPFOV;
	bool RightShoulder;

	if (ControlledPawn)
	{
		if (ICameraInterface* CameraInterface = Cast<ICameraInterface>(ControlledPawn))
		{
			PivotTarget = CameraInterface->Get3pPivotTarget();
			FPTarget = CameraInterface->GetFPCameraTarget();
			CameraInterface->GetCameraParameters(TPFOV, FPFOV, RightShoulder);
		}
	}

	// Step 2: Calculate Target Camera Rotation. Use the Control Rotation and interpolate for smooth camera rotation
	TargetCameraRotation = FMath::RInterpTo(
		GetCameraRotation(),
		GetOwningPlayerController()->GetControlRotation(),
		GetWorld()->DeltaTimeSeconds,
		GetCameraBehaviorParam(NAME_RotationLagSpeed)
	);
	
	// Step 3: Calculate the Smoothed Pivot Target (Orange Sphere). Get the 3P Pivot Target (Green Sphere) 
	// and interpolate using axis independent lag for maximum control
	const FVector LagSpeed = FVector(
		GetCameraBehaviorParam(NAME_PivotLagSpeed_X),
		GetCameraBehaviorParam(NAME_PivotLagSpeed_Y),
		GetCameraBehaviorParam(NAME_PivotLagSpeed_Z)
	);

	const FVector SmoothedPivotTargetLocation = CalculateAxisIndependentLag(
		SmoothedPivotTarget.GetLocation(),
		PivotTarget.GetLocation(),
		TargetCameraRotation,
		LagSpeed);

	SmoothedPivotTarget = FTransform(PivotTarget.GetRotation().Rotator(), SmoothedPivotTargetLocation, FVector(1.f));

	// 중심점 오프셋 값을 Camera AnimInstance 의 커브값을 통해 읽어온다
	const FVector PivotLocationOffset(
		GetCameraBehaviorParam(NAME_PivotOffset_X),
		GetCameraBehaviorParam(NAME_PivotOffset_Y),
		GetCameraBehaviorParam(NAME_PivotOffset_Z)
	);

	// 카메라 위치 오프셋 값을 Camera AnimInstance 의 커브값을 통해 읽어온다.
	const FVector CameraOffset(
		GetCameraBehaviorParam(NAME_CameraOffset_X),
		GetCameraBehaviorParam(NAME_CameraOffset_Y),
		GetCameraBehaviorParam(NAME_CameraOffset_Z)
	);
	// Step 5: Calculate Target Camera Location. Get the Pivot location and apply camera relative offsets
	// Setp 5: 카메라 최종 위치를 계산한다. 회전 기준점 위치에, 카메라의 상대적 오프셋을 더한다.
	PivotLocation = SmoothedPivotTarget.GetLocation() + SmoothedPivotTarget.GetRotation().Rotator().RotateVector(PivotLocationOffset);
	TargetCameraLocation = PivotLocation + TargetCameraRotation.RotateVector(CameraOffset);


	// Step 6: Trace for an object between the camera and character to apply a corrective offset. 
	// Trace origins are set within the Character BP via the Camera Interface. Functions like the normal spring arm, 
	// but can allow for different trace origins regardless of the pivot

	/*FVector TraceOrigin;
	float TraceRadius;
	TEnumAsByte<ETraceTypeQuery> TraceChannel;
	if (ControlledPawn && ControlledPawn->GetClass()->ImplementsInterface(UCameraInterface::StaticClass()))
	{
		ICameraInterface::Execute_Get3pTraceParams(ControlledPawn, TraceOrigin, TraceRadius, TraceChannel);
	}

	TArray<AActor*> IgnoreActors;
	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(
		this,
		TraceOrigin,
		TargetCameraLocation,
		TraceRadius,
		TraceChannel,
		false,
		IgnoreActors,
		GetTraceDebugType(EDrawDebugTrace::Type::ForOneFrame),
		HitResult,
		true
	);
	if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
	{
		TargetCameraLocation += (HitResult.Location - HitResult.TraceEnd);
	}*/

	// Step 8: Lerp First Person Override and return target camera parameters
	FTransform Result;
	Result.Blend(
		FTransform(TargetCameraRotation, TargetCameraLocation, FVector(1.f)),
		FTransform(TargetCameraRotation, FPTarget, FVector(1.f)),
		GetCameraBehaviorParam(NAME_Weight_FirstPerson)
	);


	OutLocation = Result.GetLocation();
	OutRotation = Result.GetRotation().Rotator();

	DrawDebugLine(GetWorld(), OutLocation, OutLocation + OutRotation.Vector() * 1000.f, FColor::Green, false);

	OutFOV = FMath::Lerp(TPFOV, FPFOV, GetCameraBehaviorParam(NAME_Weight_FirstPerson));
}

float ABattleSkyCameraManager::GetCameraBehaviorParam(const FName& CurveName)
{
	if (UAnimInstance* Anim = CameraBehavior->GetAnimInstance())
	{
		return Anim->GetCurveValue(CurveName);
	}
	else
	{
		return 0.f;
	}
}

FVector ABattleSkyCameraManager::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeed)
{
	const FRotator CameraRotationYaw = FRotator(0.f, CameraRotation.Yaw, 0.f);
	const FVector CurrentUnRotatedLocation = CameraRotationYaw.UnrotateVector(CurrentLocation);
	const FVector TargetUnRotatedLocation = CameraRotationYaw.UnrotateVector(TargetLocation);
	const float DeltaSeconds = GetWorld()->DeltaTimeSeconds;

	return CameraRotationYaw.RotateVector(FVector(
		FMath::FInterpTo(CurrentUnRotatedLocation.X, TargetUnRotatedLocation.X, DeltaSeconds, LagSpeed.X),
		FMath::FInterpTo(CurrentUnRotatedLocation.Y, TargetUnRotatedLocation.Y, DeltaSeconds, LagSpeed.Y),
		FMath::FInterpTo(CurrentUnRotatedLocation.Z, TargetUnRotatedLocation.Z, DeltaSeconds, LagSpeed.Z)));
}
