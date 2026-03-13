#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DragIconWidget.h"
#include "DragManager.generated.h"

UCLASS()
class BATTLESKY_API UDragManager : public UObject
{
    GENERATED_BODY()

public:

    void StartDrag(class UItemWidget* InWidget);
    void EndDrag();

    bool IsDragging() const;
    UItemWidget* GetDraggingWidget() const;

    UPROPERTY()
    UDragIconWidget* DragIconWidget;

    UPROPERTY(EditAnywhere)
	TSubclassOf<UDragIconWidget> DragIconWidgetClass;
private:

    // 현재 드래그 중인 아이템 위젯
    UPROPERTY()
    class UItemWidget* DraggingWidget = nullptr;

    // 드래그 상태
    bool bDragging = false;
};