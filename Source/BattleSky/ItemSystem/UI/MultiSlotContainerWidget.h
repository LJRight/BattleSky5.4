#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DropSlotBase.h"
#include "MultiSlotContainerWidget.generated.h"

class UScrollBox;
struct FItemViewData;

UCLASS()
class BATTLESKY_API UMultiSlotContainerWidget : public UDropSlotBase
{
	GENERATED_BODY()

public:
	void Update(const TArray<FItemViewData>& InViewDatas);
private:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* Container;
};