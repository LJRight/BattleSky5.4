// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESKY_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(class AItemBase* ItemInfo);
	AItemBase* ItemActor = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
	class UImage* Icon;
	UPROPERTY(meta = (BindWidget))
	class UBorder* Background;

	FLinearColor BaseColor = FLinearColor(.3f, .3f, .3f, .5f);
	FLinearColor HoverColor = FLinearColor(.05f, .05f, .05f, .5f);
	

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
};
