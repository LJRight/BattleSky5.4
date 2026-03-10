// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "BattleSkyPlayerController.h"
#include "BattleSkyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "InventoryComponent.h"

void AWeaponBase::Interact(ABattleSkyPlayerController* Interactor)
{
	Super::Interact(Interactor);
	if (ABattleSkyCharacter* BSCharacter = Cast<ABattleSkyCharacter>(Interactor->GetCharacter()))
	{
		//EWeaponSlot OutEquipedSlot
		if (BSCharacter->Inventory->EquipItem(this))
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon Equipeed!!"));
			OnEquipped(BSCharacter);
		}
	}
}

void AWeaponBase::OnEquipped(ABattleSkyCharacter* Character)
{
	SetOwner(Character);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttachToComponent(
		Character->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("WeaponSocket")
	);
}

FVector2D AWeaponBase::OnFire() const
{
	return FVector2D( (FMath::RandBool() ? 1.f : -1.f) * FMath::FRandRange(.5f, HorizontalRecoil), -FMath::FRandRange(.5f, VerticalRecoil));
}

void AWeaponBase::AttachToHand(USkeletalMeshComponent* TargetMesh, const FName TargetSocketName)
{
	Mesh->SetSimulatePhysics(true);
	AttachToComponent(
		TargetMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TargetSocketName
	);
}
