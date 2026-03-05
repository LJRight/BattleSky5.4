// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CompassWidget.generated.h"


class UImage;
class UMaterialInstanceDynamic;

UCLASS()
class BATTLESKY_API UCompassWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // 디자이너에서 바인딩할 Image
    UPROPERTY(meta = (BindWidget))
    UImage* CompassImage;

private:

    UMaterialInstanceDynamic* CompassMID;
    void UpdateCompass();
};
