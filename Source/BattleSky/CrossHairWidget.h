// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrossHairWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API UCrossHairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(meta = (BindWidget))
	class UBorder* Top;
	UPROPERTY(meta = (BindWidget))
	class UBorder* Bottom;
	UPROPERTY(meta = (BindWidget))
	class UBorder* Left;
	UPROPERTY(meta = (BindWidget))
	class UBorder* Right;

	bool bSpread = false;

	float SpreadAmount = 20.f;
	float TargetSpread;
	float CurrentSpread = 0.f;
	float SpreadInterpSpeed = 10.f;

	void OnSpreadChanged(bool bNewSpread);

};
