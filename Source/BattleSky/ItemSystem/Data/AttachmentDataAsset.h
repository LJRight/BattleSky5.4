#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "AttachmentDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FWeaponStatModifier
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageMultiplier = 1.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D RecoilMultiplier = FVector2D(1.0f, 1.0f);
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReloadSpeedMultiplier = 1.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimingSpeedMultiplier = 1.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AmmoBonus = 0;
};

UCLASS()
class BATTLESKY_API UAttachmentDataAsset : public UItemDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EAttachmentType AttachmentType;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponType SupportedWeaponType;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FWeaponStatModifier Modifier;
};
