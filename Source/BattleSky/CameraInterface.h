// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CameraInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCameraInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class BATTLESKY_API ICameraInterface
{
	GENERATED_BODY()

public:

	virtual FTransform Get3pPivotTarget() const = 0;
	virtual FVector GetFPCameraTarget() const = 0;
	virtual void GetCameraParameters(float& OutTP_FOV, float& OutFP_FOV, bool& OutRightShoulder) const = 0;

	//void Get3pTraceParams(FVector& OutTraceOrigin, float& OutTraceRadius, TEnumAsByte<ETraceTypeQuery>& OutTraceChannel);
};
