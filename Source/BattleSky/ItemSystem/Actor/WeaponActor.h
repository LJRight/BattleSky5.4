#pragma once

#include "CoreMinimal.h"
#include "ItemActor.h"
#include "../Data/WeaponDataAsset.h"
#include "../Data/AttachmentDataAsset.h"
#include "WeaponActor.generated.h"

USTRUCT(BlueprintType)
    struct FWeaponRuntimeStats
{
    GENERATED_BODY()

    FWeaponRuntimeStats()
        : Damage(0.f)
        , RecoilPerShot(FVector2D::ZeroVector)
        , ReloadTime(0.f)
        , AimingSpeed(0.f)
        , MaxAmmo(0)
    {
	}
    FWeaponRuntimeStats(UWeaponDataAsset* Data)
    {
        if (Data)
        {
            Damage = Data->Damage;
            RecoilPerShot = Data->RecoilPerShot;
            ReloadTime = Data->ReloadingTime;
            AimingSpeed = Data->AimingSpeed;
            MaxAmmo = Data->MaxAmmo;
        }
	}
    UPROPERTY(VisibleAnywhere)
    float Damage;

    UPROPERTY(VisibleAnywhere)
    FVector2D RecoilPerShot = FVector2D(1.0f, 1.0f);

    UPROPERTY(VisibleAnywhere)
    float ReloadTime;

    UPROPERTY(VisibleAnywhere)
    float AimingSpeed;
    UPROPERTY(VisibleAnywhere)
	int32 MaxAmmo;

    void Update(const FWeaponStatModifier& Modifier)
    {
        Damage *= Modifier.DamageMultiplier;
        RecoilPerShot *= Modifier.RecoilMultiplier;
		ReloadTime *= Modifier.ReloadSpeedMultiplier;
		AimingSpeed *= Modifier.AimingSpeedMultiplier;
		MaxAmmo += Modifier.AmmoBonus;
    }
};

UCLASS()
class BATTLESKY_API AWeaponActor : public AItemActor
{
	GENERATED_BODY()
public:
    AWeaponActor();

protected:
    /* ===================== */
    /* ===== Runtime ======= */
    /* ===================== */

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWeaponRuntimeStats CachedStats;
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentAmmo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<EAttachmentType, UAttachmentDataAsset*> EquippedAttachments;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EFireMode CurrentFireMode;
    
protected:
    virtual void BeginPlay() override;

public:

    void Fire();
	FORCEINLINE void EndFire() { bTriggerHeld = false; };
    void Reload();

    void AddAttachment(UAttachmentDataAsset* Attachment);
	bool CanAttach(UAttachmentDataAsset* Attachment);
    void RemoveAttachment(UAttachmentDataAsset* Attachment);

protected:

    void RecalculateStats();

	bool bCanFire;
	bool bTriggerHeld = false;
	FTimerHandle FireTimer;
    bool CanFire();
    /* ===================== */
    /* ===== Helpers ======= */
    /* ===================== */

    FORCEINLINE UWeaponDataAsset* GetWeaponData() const
    {
        return Cast<UWeaponDataAsset>(ItemData);
	}
};