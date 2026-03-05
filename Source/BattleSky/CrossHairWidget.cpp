// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossHairWidget.h"
#include "Components/Border.h"

#include "BattleSkyCharacter.h"

void UCrossHairWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCrossHairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ABattleSkyCharacter* Char = GetOwningPlayerPawn<ABattleSkyCharacter>())
	{
		TargetSpread = Char->Get_HasMovementInput() ? 40.f : 0.f;

		CurrentSpread = FMath::FInterpTo(
			CurrentSpread,
			TargetSpread,
			InDeltaTime,
			SpreadInterpSpeed
		);
		if (Top && Bottom && Left && Right)
		{
			Top->SetRenderTranslation(FVector2D(0, -CurrentSpread));
			Bottom->SetRenderTranslation(FVector2D(0, CurrentSpread));
			Left->SetRenderTranslation(FVector2D(-CurrentSpread, 0));
			Right->SetRenderTranslation(FVector2D(CurrentSpread, 0));
		}
	}
}

void UCrossHairWidget::OnSpreadChanged(const bool bSpreaded)
{

}