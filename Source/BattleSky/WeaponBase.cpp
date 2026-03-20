#include "WeaponBase.h"
#include "BattleSkyPlayerController.h"
#include "BattleSkyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryComponent.h"

const static FVector G = FVector(0.f, 0.f, -980.f);

AWeaponBase::AWeaponBase()
{
	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Point"));
	Muzzle->SetupAttachment(Mesh);
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 상호작용 가능한지 여부를 반환한다
bool AWeaponBase::TryInteract()
{	
	if (CanInteract)
	{
		CanInteract = false;
		return true;
	}
	return false;
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

void AWeaponBase::AttachToCharacter(USkeletalMeshComponent* TargetMesh, const FName TargetSocketName, bool IsHand)
{
	AttachToComponent(
		TargetMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TargetSocketName
	);

	Mesh->SetRelativeLocation(IsHand ? HandAttach.LocationOffset : BodyAttach.LocationOffset);
	Mesh->SetRelativeRotation(IsHand ? HandAttach.RotaionOffset : BodyAttach.RotaionOffset);
}

//{
//	"Tagged": [
//		[
//			"RelativeLocation",
//			"(X=-2.007300,Y=0.170892,Z=-3.206331)"
//		],
//			[
//				"RelativeRotation",
//				"(Pitch=22.562997,Yaw=-0.538000,Roll=1.259965)"
//			],
//			[
//				"RelativeScale3D",
//				"(X=1.000003,Y=1.000001,Z=1.000000)"
//			],
//			[
//				"Mobility",
//				"Movable"
//			]
//	]
//}