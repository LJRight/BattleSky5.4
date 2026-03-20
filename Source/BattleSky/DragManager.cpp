// Fill out your copyright notice in the Description page of Project Settings.


#include "DragManager.h"
#include "ItemWidget.h"
#include "BattleSkyCharacter.h"

// 인벤토리에서 드래그 시작 시 호출
void UDragManager::StartDrag(UItemWidget* InWidget)
{
    if (!InWidget)
    {
        return;
    }
	DraggingItemWidget = InWidget;
	/*DraggingItem = InWidget->GetItem();
	SourceSlotType = InWidget->GetOwnerSlotType();*/
    bDragging = true;
	

    if (DragIconWidgetClass)
    {
		DragIconWidget = CreateWidget<UDragIconWidget>(InWidget->GetOwningPlayer(), DragIconWidgetClass);
        if (DragIconWidget)
        {
			DragIconWidget->SetIcon(InWidget->Icon);
			// 마우스 이벤트 무시 (아래 위젯이 입력 받도록)
            DragIconWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
            DragIconWidget->AddToViewport(9999);
		}
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Drag Icon Widget Class"));
    }
}

void UDragManager::EndDrag()
{

	bDragging = false;
    // 드래깅 표시용 아이콘 위젯 제거
    if (DragIconWidget)
    {
        DragIconWidget->RemoveFromParent();
        DragIconWidget = nullptr;
	}

	DraggingItemWidget = nullptr;
	//DraggingItem = nullptr;

	//if (CanDragAction())
 //   {
 //       // 시작 슬롯 판단 여부는 GetOwner 로 가능할 지도

 //       // 이건 시작 슬롯이 바닥이었다는 의미
 //       if (DraggingItem->GetOwner() == nullptr)
 //       {
 //           /*
 //               시작 슬롯이 바닥이었을 때 가능한 시나리오는 도착 슬롯 종류에 따라
 //               1. 인벤토리에 넣기
 //               2. 캐릭터가 장착하기
 //               3. 총기 장착하기
 //               4. 총기 파츠 부착하기 로 나눌 수 있다
 //           
 //           */
 //           if (CurrentTargetSlotAcceptType.Num() > 1)
 //           {
 //               if (ABattleSkyCharacter* BSCharacter = Cast<ABattleSkyCharacter>(DraggingWidget->GetOwningPlayer()->GetCharacter()))
 //               {
	//				BSCharacter->Server_DoInteraction(DraggingItem);
 //               }
 //           }
 //           else
 //           {

 //           }
	//		
 //       }
 //       /* 소유가 있다->캐릭터일 경우 인벤토리 슬롯, 헬멧, 조끼 등 방어구 슬롯, 총 슬롯
 //          -> 총일 경우 파츠 슬롯
 //          중요한 건 도착 슬롯이 받아드릴 수 있는 아이템 타입 수에 따라, 종류에 따라 
 //          1. 바닥에 버리기
 //          2. 인벤토리로 옮기기
 //          3. 파츠 장착
 //        
 //       
 //       
 //       
 //       
 //       
 //       */
 //       else
 //       {

 //       }
 //   }
}

//bool UDragManager::CanDragAction() const
//{
//    // 드래그 중인 위젯/아이템이 있는지, 아이템과 상호작용할 수 있는지, 
//    // 현재 타깃 슬롯이 있는지(타깃 슬롯에서 받아들일 수 있는 아이템 타입 리스트로 확인)
//    // 현재 타깃 슬롯이 해당 아이템 타입을 받을 수 있는지
//
//	return DraggingWidget && DraggingItem && !CurrentTargetSlotAcceptType.IsEmpty() && CurrentTargetSlotAcceptType.Contains(DraggingItem->Type);
//}
