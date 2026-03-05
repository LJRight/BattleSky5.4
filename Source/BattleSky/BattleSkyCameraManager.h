#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "BattleSkyCameraManager.generated.h"

UCLASS()
class BATTLESKY_API ABattleSkyCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	ABattleSkyCameraManager();
	// void OnPossess(APawn* NewPawn);

	UFUNCTION(BlueprintCallable)
	void CustomCameraBehavior(FVector& OutLocation, FRotator& OutRotation, float& OutFOV);
	float GetCameraBehaviorParam(const FName& CurveName);
	FVector CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeed);

	

protected:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* CameraBehavior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APawn* ControlledPawn = nullptr;
	
	FVector RootLocation;
	FTransform SmoothedPivotTarget;
	FVector PivotLocation;
	FVector TargetCameraLocation;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FRotator TargetCameraRotation;

private:
	// FreeLook Returning
	float ReturnDuration = 2.f;
	bool bDelegateBound = false;
	bool bIsReturningFromFreeLook = false;
	FRotator FreeLookStartControlRotation;
	void OnFreeLookActionStart(bool IsFreeLooking, FRotator TargetRotation);
	void OnFreeLookReturnFinished();
	FTimerHandle FreeLookReturnTimerHandle;
};
