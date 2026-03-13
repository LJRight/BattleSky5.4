// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "BattleSkyPlayerController.h"
#include "BattleSkyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryComponent.h"

const static FName NAME_Primary_Socket("Primary");
const static FName NAME_Secondary_Socket("Secondary");

const static TArray<FName> WeaponSockets = { NAME_Primary_Socket , NAME_Secondary_Socket };

const static FVector G = FVector(0.f, 0.f, -980.f);

AWeaponBase::AWeaponBase()
{
	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Point"));
	Muzzle->SetupAttachment(RootComponent);
}

void AWeaponBase::Interact(ABattleSkyPlayerController* Interactor)
{
	Super::Interact(Interactor);
	if (ABattleSkyCharacter* BSCharacter = Cast<ABattleSkyCharacter>(Interactor->GetCharacter()))
	{
		//EWeaponSlot OutEquipedSlot
		int SlotIndex;
		if (BSCharacter->Inventory->EquipItem(this, SlotIndex))
		{
			OnEquipped(BSCharacter, SlotIndex);
		}
	}
}

void AWeaponBase::OnEquipped(ABattleSkyCharacter* Character, int SlotIndex)
{
	SetOwner(Character);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttachToComponent(
		Character->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		WeaponSockets[SlotIndex]
	);

	Mesh->SetRelativeLocation(BackAttach.LocationOffset);
	Mesh->SetRelativeRotation(BackAttach.RotaionOffset);
}

bool AWeaponBase::OnFire(FVector2D& OutRecoil, const FVector Start, const FRotator Rotation)
{
	if (CanFire)
	{
		// 카메라 기준 발사 목표 위치 구하기
		FVector End = Start + Rotation.Vector() * 800000.f;
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(
			Hit,
			Start,
			End,
			ECC_Visibility
		);

		FVector st = Muzzle->GetComponentLocation();
		FVector ShootDirection = Hit.bBlockingHit ? (Hit.ImpactPoint - st).GetSafeNormal() : FRotationMatrix(st.Rotation()).GetUnitAxis(EAxis::X);
		FVector Velocity = ShootDirection * BulletSpeed;
		float Step = 0.01f;
		FVector PrevPos = st;
		for (float time = 0.f; time <= 10.f; time += Step)
		{
			FVector NewPos = PrevPos + Velocity * Step;
			Velocity += G * Step;
			FHitResult HitResult;

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				PrevPos,
				NewPos,
				ECC_Visibility
			);

			DrawDebugLine(GetWorld(), PrevPos, NewPos, FColor::Green, false, 2.f);

			if (bHit)
			{
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 12, FColor::Red, false, 5.f);
				break;
			}

			PrevPos = NewPos;
		}

		// DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Red, false, 1.f);
		// 발사 로직
		
		// 무작위 반동 생성
		OutRecoil = FVector2D((FMath::RandBool() ? 1.f : -1.f) * FMath::FRandRange(.5f, HorizontalRecoil), -FMath::FRandRange(.5f, VerticalRecoil));
		

		// 총 소리 재생
		if (GunSound)
		{

			UGameplayStatics::PlaySoundAtLocation(this, GunSound, GetActorLocation());
		}

		CanFire = false;
		GetWorldTimerManager().SetTimer(RefireTimerHandle, this, &AWeaponBase::Test, FiringRate);
		return true;
	}

	
	return false;
}
void AWeaponBase::Test()
{
	CanFire = true;
}

void AWeaponBase::AttachToHand(USkeletalMeshComponent* TargetMesh, const FName TargetSocketName)
{
	Mesh->SetSimulatePhysics(true);
	AttachToComponent(
		TargetMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TargetSocketName
	);

	Mesh->SetRelativeLocation(HandAttach.LocationOffset);
	Mesh->SetRelativeRotation(HandAttach.RotaionOffset);
}
//(X = -3.386755, Y = -0.144507, Z = -3.410217)
//(Pitch = 19.951726, Yaw = -0.534137, Roll = 1.199060)
