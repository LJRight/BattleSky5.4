#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UMultiSlotContainerWidget;
class USingleSlotContainerWidget;
struct FInventoryItemData;
class UItemDataAsset;
class UProgressBar;
class AItemActor;

// �����ۿ� ���� UI(����)�� �ʿ��� View Data ���� 
// ���� �������� ��� Ư���ϱ� ���� ���� ������
// �κ��丮 �������� ��� Ư���ϱ� ���� FGuid ��
USTRUCT(BlueprintType)
struct FItemViewData 
{
	GENERATED_BODY()
public:
	FItemViewData(FGuid ID = FGuid(), UItemDataAsset* ItemData = nullptr, AActor* ItemActor = nullptr)
		: ID(ID), ItemData(ItemData), ItemActor(ItemActor)
	{
	};
	FGuid ID;
	UItemDataAsset* ItemData = nullptr;
	AActor* ItemActor = nullptr;
};

UCLASS()
class BATTLESKY_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init();
	void NearByUpdate(const TArray<AItemActor*>& WorldItemActors);
	void InventoryUpdate(const TArray<FInventoryItemData>& InventoryItemDatas, float WeightRatio);
private:

	UPROPERTY(meta=(BindWidget))
	UMultiSlotContainerWidget* NearByList;
	UPROPERTY(meta = (BindWidget))
	UMultiSlotContainerWidget* InventoryList;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* InventorySpace;

	UPROPERTY(meta = (BindWidget))
	USingleSlotContainerWidget* PrimaryWeaponSlot;
	UPROPERTY(meta = (BindWidget))
	USingleSlotContainerWidget* SecondaryWeaponSlot;
	UPROPERTY(meta = (BindWidget))
	USingleSlotContainerWidget* SidearmWeaponSlot;
	UPROPERTY(meta = (BindWidget))
	USingleSlotContainerWidget* MeleeWeaponSlot;
	UPROPERTY(meta = (BindWidget))
	USingleSlotContainerWidget* ThrowableWeaponSlot;

	UPROPERTY()
	TArray<USingleSlotContainerWidget*> WeaponSlots;
};
