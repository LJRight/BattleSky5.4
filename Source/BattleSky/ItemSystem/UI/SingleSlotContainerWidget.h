#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DropSlotBase.h"

#include "SingleSlotContainerWidget.generated.h"

struct FItemViewData;


UCLASS()
class BATTLESKY_API USingleSlotContainerWidget : public UDropSlotBase
{
	GENERATED_BODY()

public:
	void Add(const FItemViewData& ViewData);
	void Remove();

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* Container;
	UWidget* CurrentChild;
};