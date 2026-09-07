#include "MultiSlotContainerWidget.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"

#include "../Source/BattleSky/InventoryWidget.h"
#include "DraggableWidgetBase.h"

void UMultiSlotContainerWidget::Update(const TArray<FItemViewData>& InViewDatas)
{
	if (SlotElementWidgetClass && Container)
	{
		Container->ClearChildren();
		for (const FItemViewData& ViewData : InViewDatas)
		{
			UDraggableWidgetBase* ElementWidget = CreateWidget<UDraggableWidgetBase>(GetOwningPlayer(), SlotElementWidgetClass);
			if (ElementWidget)
			{
				ElementWidget->Init(SlotInfo, ViewData.ID, ViewData.ItemData, ViewData.ItemActor);
				UPanelSlot* SlotElement = Container->AddChild(ElementWidget);
				if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(SlotElement))
				{
					ScrollBoxSlot->SetPadding(FMargin(0.f, 5.f));
				}
			}
		}
	}
}
