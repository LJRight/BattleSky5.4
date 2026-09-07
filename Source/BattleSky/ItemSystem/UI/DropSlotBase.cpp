#include "DropSlotBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "../Source/BattleSky/BattleSkyPlayerController.h"
#include "../Source/BattleSky/ItemSystem/Data/ItemDragPayload.h"

bool UDropSlotBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!InOperation)
	{
		return false;
	}
	if (UItemDragPayload* CurrentPayload = Cast<UItemDragPayload>(InOperation->Payload))
	{
		const FSlotInfo& SourceSlotType = CurrentPayload->SourceSlotInfo;
		if (ABattleSkyPlayerController* Controller = Cast<ABattleSkyPlayerController>(GetOwningPlayer()))
		{
			if (CurrentPayload->WorldActor)
			{
				Controller->HandleInventoryLogic(SourceSlotType, SlotInfo, CurrentPayload->WorldActor);
			}
			else
			{
				Controller->HandleInventoryLogic(SourceSlotType, SlotInfo, CurrentPayload->ID);
			}
		}
	}
	return true;
}
