// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"
#include "Components/SphereComponent.h"
// Sets default values
AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	
	RootComponent = InteractionSphere;


	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));;
	Mesh->SetupAttachment(RootComponent);

	

	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionObjectType(ECC_GameTraceChannel1);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);

	InteractionSphere->InitSphereRadius(InteractSphereRadius);
}

void AItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemBase::Interact(ABattleSkyPlayerController* Interactor)
{
}

void AItemBase::EnableInteraction(bool bEnable)
{
	InteractionSphere->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	InteractionSphere->SetGenerateOverlapEvents(bEnable);
}

FText AItemBase::GetText() const
{
	return FText::FromString(Name);
}

