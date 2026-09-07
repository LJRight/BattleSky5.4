// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// #include "ItemSystem/Data/ItemDataAsset.h"
#include "ItemSystem/Data/WeaponDataAsset.h"
#include "ItemSystem/Data/ItemSlotType.h"
#include "InventoryComponent.generated.h"

class AItemActor;
class AWeaponActor;
class UAttachmentDataAsset;

// 인벤토리 내부에서 관리하는 아이템 데이터 (정적 아이템 데이터와, 동적인 아이템 수량을 관리한다)
USTRUCT()
struct FInventoryItemData
{
	GENERATED_BODY()
public:
	FInventoryItemData(UItemDataAsset* Data = nullptr, int32 Count = 1, FGuid ID = FGuid()) : Data(Data), Count(Count), ID(ID)
	{
	};
	UPROPERTY()
	UItemDataAsset* Data = nullptr;
	UPROPERTY()
	int32 Count;
	UPROPERTY()
	FGuid ID;

	bool operator<(const FInventoryItemData& Other) const
	{
		if (!Data || !Other.Data)
		{
			return Data < Other.Data;
		}
		if (Data->Type != Other.Data->Type)
		{
			return Data->Type < Other.Data->Type;
		}
		if (Data->ItemID != Other.Data->ItemID)
		{
			return Data->ItemID.LexicalLess(Other.Data->ItemID);
		}
		return Count < Other.Count;
	};
	bool operator==(const UItemDataAsset* Other) const
	{
		return Data == Other;
	};
};


USTRUCT()
struct FInventoryWeaponData
{
	GENERATED_BODY()
public:
	FInventoryWeaponData(UWeaponDataAsset* InData = nullptr, int32 InCurrentAmmo = 0, TMap<EAttachmentType, UAttachmentDataAsset*> InEquippedAttachments = {})
		: Data(InData), CurrentAmmo(InCurrentAmmo), EquippedAttachments(InEquippedAttachments) 
	{
	};
	UPROPERTY()
	UWeaponDataAsset* Data = nullptr;
	UPROPERTY()
	int32 CurrentAmmo;
	UPROPERTY()
	TMap<EAttachmentType, UAttachmentDataAsset*> EquippedAttachments;
};


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryUpdated, const TArray<FInventoryItemData>&, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponSlotUpdated, const TArray<FInventoryWeaponData>&);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BATTLESKY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UInventoryComponent(); 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool AddItem(AItemActor* ItemActor);
	bool DropItem(const FGuid TargetID);

	void EquipWeapon(AWeaponActor* WeaponActor);
	void DropWeapon(const FInventoryWeaponData& DropTarget);
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_InventoryItems)
	TArray<FInventoryItemData> InventoryItems;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_WeaponSlots)
	TArray<FInventoryWeaponData> WeaponSlots;
	FORCEINLINE bool IsWeaponSlotEmpty(int32 Index) const { return WeaponSlots[Index].Data == nullptr; };

	UPROPERTY(Replicated, EditDefaultsOnly)
	float MaxWeight = 35.f;

	UPROPERTY(Replicated)
	float CurrentWeight = 0.f;

	UFUNCTION()
	FORCEINLINE void OnRep_InventoryItems() const { if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast(InventoryItems, CurrentWeight / MaxWeight); };
	FOnInventoryUpdated OnInventoryUpdated;
	UFUNCTION()
	FORCEINLINE void OnRep_WeaponSlots() const { if (OnWeaponSlotUpdated.IsBound()) OnWeaponSlotUpdated.Broadcast(WeaponSlots); };
	FOnWeaponSlotUpdated OnWeaponSlotUpdated;

	//UPROPERTY()
	//TArray<AWeaponBase*> WeaponSlots;

	//bool EquipItem(AItemActor* Item);
	//bool EquipWeapon(AWeaponBase* Weapon);

	FORCEINLINE bool CanPickup(float AddedWeight) const { return CurrentWeight + AddedWeight <= MaxWeight; };

	//UFUNCTION()
	//FORCEINLINE AItemActor* GetWeapon(const int32 Index) const { return WeaponSlots[Index]; };
};
