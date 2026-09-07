#include "SingleSlotContainerWidget.h"
#include "DraggableWidgetBase.h"
#include "Components/PanelWidget.h"
#include "../Source/BattleSky/InventoryWidget.h"

void USingleSlotContainerWidget::Add(const FItemViewData& ViewData)
{
	if (!Container || !SlotElementWidgetClass) { 
		return;
	}
    Remove();

    UDraggableWidgetBase* ElementWidget =
        CreateWidget<UDraggableWidgetBase>(GetOwningPlayer(), SlotElementWidgetClass);

    if (!ElementWidget) return;

    ElementWidget->Init(
		SlotInfo,
        ViewData.ID,
        ViewData.ItemData,
        ViewData.ItemActor);

	Container->AddChild(ElementWidget);
	CurrentChild = ElementWidget;
}

void USingleSlotContainerWidget::Remove()
{
    if (CurrentChild && Container)
    {
		Container->RemoveChild(CurrentChild);
		CurrentChild = nullptr;
    }
}

