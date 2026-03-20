// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponBase.h"
#include "InventoryComponent.generated.h"


UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Sidearm UMETA(DisplayName = "Sidearm"),
	Melee UMETA(DisplayName = "Melee"),
	MAX,
};

UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	Helmet UMETA(DisplayName = "Helmet"),
	Vest UMETA(DisplayName = "Vest"),

	MAX,
};

class AItemBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BATTLESKY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY()
	TArray<AWeaponBase*> WeaponSlots;

	UPROPERTY()
	TArray<AItemBase*> InventoryItems;

	bool EquipItem(AItemBase* Item);
	bool EquipWeapon(AWeaponBase* Weapon);

	void PickUpItem(AItemBase* TargetItem);
	FORCEINLINE bool CanPickUp(float AddedWeight) const { return CurrentWeight + AddedWeight <= MaxWeight; };

	UFUNCTION()
	FORCEINLINE AWeaponBase* GetWeapon(const int32 Index) const { return WeaponSlots[Index]; };

	UPROPERTY(EditDefaultsOnly)
	float MaxWeight = 35.f;

	UPROPERTY()
	float CurrentWeight = 0.f;

private:
	class ABattleSkyCharacter* OwningCharacter;
};
