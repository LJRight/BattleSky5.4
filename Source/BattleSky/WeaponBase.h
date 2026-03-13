// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "WeaponBase.generated.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AR UMETA(DisplayName = "AR"),
	SR UMETA(DisplayName = "SR"),
	DMR UMETA(DisplayName = "DMR"),
	Pistol UMETA(DisplayName = "Pistol"),
	ShotGun UMETA(DisplayName = "ShotGun"),
	Melee UMETA(DisplayName = "Melee"),
};

USTRUCT(BlueprintType)
struct FAttachOffset
{
	GENERATED_BODY()
	FAttachOffset(FVector Loc = FVector::ZeroVector, FRotator Rot = FRotator::ZeroRotator) : LocationOffset(Loc), RotaionOffset(Rot) {};
	UPROPERTY(EditAnywhere)
	FVector LocationOffset;
	UPROPERTY(EditAnywhere)
	FRotator RotaionOffset;
};

class ABattleSkyCharacter;

UCLASS()
class BATTLESKY_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()

public:
	AWeaponBase();
	// virtual void Tick(float DeltaTime) override;
	virtual void Interact(ABattleSkyPlayerController* Interactor) override;

	void OnEquipped(ABattleSkyCharacter* Character, int SlotIndex);

	bool OnFire(FVector2D& OutRecoil, /*const FVector TargetLocation, */const FVector Start, const FRotator Rotation);
	void Test();

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Muzzle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float Damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float FiringRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float Recoil;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float StoppingPower;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float BulletSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float ReloadingSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float MOA;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float VerticalRecoil;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float HorizontalRecoil;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Specification")
	float MaxAmmoCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Type")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly)
	USoundWave* GunSound;

	bool CanFire = true;
	FTimerHandle RefireTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Detail Settings")
	FAttachOffset HandAttach;
	UPROPERTY(EditAnywhere, Category = "Detail Settings")
	FAttachOffset BackAttach;

	void AttachToHand(USkeletalMeshComponent* TargetMesh, const FName TargetSocketName);
};
