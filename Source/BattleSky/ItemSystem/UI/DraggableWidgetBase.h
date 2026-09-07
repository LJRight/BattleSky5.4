#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Source/BattleSky/ItemSystem/Data/ItemSlotType.h"
#include "DraggableWidgetBase.generated.h"

class UItemDataAsset;
class UItemDragPayload;
class UDragIconWidget;

UCLASS()
class BATTLESKY_API UDraggableWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:

	void Init(FSlotInfo SlotInfo, FGuid ID = FGuid(), UItemDataAsset* ItemData = nullptr, AActor* ItemActor = nullptr);

	UPROPERTY()
	UItemDragPayload* Payload = nullptr;
	UPROPERTY()
	FSlotInfo SlotInfo;
	UPROPERTY()
	FGuid ID;
	UPROPERTY()
	UItemDataAsset* ItemData = nullptr;
	UPROPERTY()
	AActor* ItemActor = nullptr;

	UPROPERTY()
	TSubclassOf<UDragIconWidget> DragIconWidgetClass;
	UPROPERTY()
	UDragIconWidget* DragIconWidget;
protected:
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	

	// void ShowDescriptionWidget(const bool bShowing);

	// ESlotType OwnerSlot;
	// 대충 아이템 설명 텍스트 띄우는 위젯 클래스 변수 UItemDescriptionWidget* Description;
	// UDragManager* DragManager;
};
