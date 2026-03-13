// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API UDragIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
