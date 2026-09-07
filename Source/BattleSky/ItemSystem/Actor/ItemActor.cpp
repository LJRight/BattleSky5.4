// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemActor.h"
#include "Net/UnrealNetwork.h"
#include "../Data/ItemDataAsset.h"
#include "Components/SphereComponent.h"

#include "../Source/BattleSky/BattleSkyPlayerController.h"

#define DEBUG_MSG(Text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, Text);

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);


	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionObjectType(ECC_GameTraceChannel1);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);

	InteractionSphere->InitSphereRadius(InteractSphereRadius);
}

void AItemActor::InitItem(UItemDataAsset* InData, int32 InQuantity)
{
	ItemData = InData;
	Quantity = InQuantity;
	if (ItemData && ItemData->WorldMesh)
	{
		Mesh->SetStaticMesh(ItemData->WorldMesh);
	}
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemActor, ItemData);
	DOREPLIFETIME(AItemActor, Quantity);
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	if (ItemData && ItemData->WorldMesh)
	{
		Mesh->SetStaticMesh(ItemData->WorldMesh);
	}
}

void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FText AItemActor::GetText() const
{
	if (ItemData)
	{
		return ItemData->Name;
	}
	return FText::GetEmpty();
}

AActor* AItemActor::Interact()
{
	if (!bCanInteract)
	{
		return nullptr;
	}
	bCanInteract = false;
	return this;
}

