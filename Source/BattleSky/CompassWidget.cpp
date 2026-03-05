#include "CompassWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"

void UCompassWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CompassImage)
	{
        CompassMID = CompassImage->GetDynamicMaterial();
    }
}

void UCompassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateCompass();
}

void UCompassWidget::UpdateCompass()
{
    if (!CompassMID)
        return;
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
        return;
	float Yaw = PC->GetControlRotation().Yaw;
    // North 기준으로 보정
	Yaw -= 79.f;

    Yaw = FMath::Fmod(Yaw + 360.f, 360.f);

    float Offset = -(Yaw / 360.f);

	CompassMID->SetScalarParameterValue("YawOffset", -Offset);
}