#include "WeaponActor.h"

AWeaponActor::AWeaponActor()
{
	
}

void AWeaponActor::BeginPlay()
{
}

void AWeaponActor::Fire()
{
	if (!CanFire())
	{
		return;
	}
	bTriggerHeld = true;
	bCanFire = false;

	CurrentAmmo--;
	GetWorldTimerManager().SetTimer(
		FireTimer,
		[this](){ bCanFire = true; },
		GetWeaponData()->FiringRate,
		false
	);
}

void AWeaponActor::Reload()
{

}

void AWeaponActor::AddAttachment(UAttachmentDataAsset* Attachment)
{
	if (!Attachment || !CanAttach(Attachment))
	{
		return;
	}
	EquippedAttachments.Add(Attachment->AttachmentType, Attachment);
	RecalculateStats();
}

bool AWeaponActor::CanAttach(UAttachmentDataAsset* Attachment)
{
	if (UWeaponDataAsset* Data = GetWeaponData())
	{
		return Data->AllowedAttachments.Contains(Attachment->AttachmentType) && !EquippedAttachments.Contains(Attachment->AttachmentType);
	}
	return false;
}

void AWeaponActor::RemoveAttachment(UAttachmentDataAsset* Attachment)
{
	
}

void AWeaponActor::RecalculateStats()
{
	if (UWeaponDataAsset* Data = GetWeaponData())
	{
		FWeaponRuntimeStats NewStats = FWeaponRuntimeStats(Data);
		for (const auto& Pair : EquippedAttachments)
		{
			NewStats.Update(Pair.Value->Modifier);
		}
		CachedStats = NewStats;
	}
}

bool AWeaponActor::CanFire()
{
	if (CurrentAmmo <= 0)
	{
		return false;
	}
	if (!bCanFire)
	{
		return false;
	}
	if (CurrentFireMode == EFireMode::SemiAuto && bTriggerHeld)
	{
		return false;
	}

	return false;
}
