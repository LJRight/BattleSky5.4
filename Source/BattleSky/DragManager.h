#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DragIconWidget.h"

//#include "ItemBase.h"
//#include "SlotWidget.h"

#include "DragManager.generated.h"

class UItemWidget;

UCLASS()
class BATTLESKY_API UDragManager : public UObject
{
    GENERATED_BODY()

public:

	void StartDrag(UItemWidget* InWidget);
    void EndDrag();

	FORCEINLINE bool IsDragging() const { return bDragging; };
	FORCEINLINE UItemWidget* GetDraggingItemWidget() const { return DraggingItemWidget; };
	/*FORCEINLINE AItemBase* GetDraggingItem() const { return DraggingItem; };
	FORCEINLINE ESlotType GetSourceSlot() const { return SourceSlotType; }*/

    UPROPERTY()
    UDragIconWidget* DragIconWidget;

    UPROPERTY(EditAnywhere)
	TSubclassOf<UDragIconWidget> DragIconWidgetClass;

	/* 
    TArray<EItemType> CurrentTargetSlotAcceptType;
	FORCEINLINE void SetCurrentSlotType(const TArray<EItemType>& Target) { CurrentTargetSlotAcceptType = Target; };
	FORCEINLINE void ClearCurrentSlotType() { CurrentTargetSlotAcceptType.Empty(); };
	*/
private:
	// 현재 드래그 중인 아이템
	UItemWidget* DraggingItemWidget;
	/*ESlotType SourceSlotType;
	AItemBase* DraggingItem = nullptr;*/
    // 드래그 상태
	bool bDragging = false;
};