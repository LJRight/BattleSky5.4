#include "DraggableWidgetBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "../Data/ItemDragPayload.h"
#include "../Source/BattleSky/DragIconWidget.h"
#include "../Source/BattleSky/ItemSystem/Data/ItemDataAsset.h"

void UDraggableWidgetBase::Init(FSlotInfo InSlotInfo, FGuid InID, UItemDataAsset* InItemData, AActor* InItemActor)
{
	SlotInfo = InSlotInfo;
	ID = InID;
	if (InItemActor)
	{
		ItemActor = InItemActor;
	}
	if (InItemData)
	{
		ItemData = InItemData;
	}
	// Payload 생성
	Payload = NewObject<UItemDragPayload>(this);
	Payload->SourceSlotInfo = SlotInfo;
	Payload->ID = ID;
	if (ItemActor)
	{
		Payload->WorldActor = ItemActor;
	}
}

// 위젯에 마우스 호버 시 해당 아이템의 설명 위젯을 띄운다
void UDraggableWidgetBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

}

// 마우스 호버 종료시 설명 위젯을 지운다
void UDraggableWidgetBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{

}

// 마우스 왼쪽 버튼 클릭 감지시, 실제 드래그 되는 (임시이름) UDraggingWidget 을 생성한다
// 해당 위젯이 실제 드래그 중 마우스 포인터 위치를 따라 이동하는 시각적 표현을 한다
// 마우스 오른쪽 버튼이 눌릴 때는 또 다른 동작을 구상 중 (배그처럼 하려면, 현재 위치한 슬롯에 따라 (버리기 or 줍기 를 할 수 있는 버튼이 포함된 위젯이 띄워진다)
// 추가로 가능하다면 배그처럼 왼쪽 Ctrl + 마우스 왼쪽 버튼일 시 아이템 수량을 정해서 줍거나 버리는 동작도 구현하고픔
FReply UDraggableWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		DragIconWidget = CreateWidget<UDragIconWidget>(this, DragIconWidgetClass);
		return UWidgetBlueprintLibrary::DetectDragIfPressed(
			InMouseEvent,
			this,	 
			EKeys::LeftMouseButton
		).NativeReply;
	}
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		// 
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UDraggableWidgetBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
	DragOp->Payload = Payload;

	// Drag Visual (임시: 자기 자신)
	DragOp->DefaultDragVisual = DragIconWidget;
	DragOp->Pivot = EDragPivot::MouseDown;
	
	OutOperation = DragOp;
}
