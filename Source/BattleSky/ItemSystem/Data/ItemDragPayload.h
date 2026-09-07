#pragma once

#include "Coreminimal.h"
#include "ItemSlotType.h"
#include "ItemDragPayload.generated.h"

UCLASS()
class BATTLESKY_API UItemDragPayload : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FSlotInfo SourceSlotInfo;		// 드래그 시작 슬롯 판별용
	UPROPERTY()
	AActor* WorldActor = nullptr;	// 월드에 있는 아이템 참조
	UPROPERTY()
	FGuid ID;						// 인벤토리 아이템 참조용
};