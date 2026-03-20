// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
// #include "Components/SphereComponent.h"
#include "ItemBase.generated.h"

class APlayerController;
class USphereComponent;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Consumable UMETA(DisplayName = "Consumable"),
	Attachment UMETA(DisplayName = "Attachment"),
	Ammo UMETA(DisplayName = "Ammo"),
	Armor UMETA(DisplayName = "Armor"),
	Throwable UMETA(DisplayName = "Throwable"),
};

UCLASS()
class BATTLESKY_API AItemBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	

	AItemBase();
	UPROPERTY(EditDefaultsOnly, Category = "Basic Information")
	FString Name;
	UPROPERTY(EditDefaultsOnly, Category = "Basic Information")
	EItemType Type;
	UPROPERTY(EditDefaultsOnly, Category = "Basic Information")
	UTexture2D* Icon;
	UPROPERTY(EditDefaultsOnly, Category = "Basic Information")
	float Weight;
	UPROPERTY(EditDefaultsOnly, Category = "Basic Information")
	int32 MaxStack;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionSphere;

	UPROPERTY(Replicated, EditAnywhere)
	int32 Count = 1;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "Trace Settings")
	float InteractSphereRadius = 10.f;

public:
	virtual void Tick(float DeltaTime) override;
	virtual bool TryInteract() override;
	virtual void EnableInteraction(bool bEnable) override;
	virtual FText GetText() const override;
};
