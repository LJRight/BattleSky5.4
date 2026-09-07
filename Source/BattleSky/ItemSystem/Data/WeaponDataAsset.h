#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "WeaponDataAsset.generated.h"


UCLASS()
class BATTLESKY_API UWeaponDataAsset : public UItemDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float FiringRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BulletSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MaxAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ReloadingTime;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D RecoilPerShot;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimingSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EWeaponType WeaponType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EAmmoType UsingAmmoType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<EAttachmentType> AllowedAttachments;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EFireMode> AllowedFireModes;
};