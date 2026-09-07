// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Source/BattleSky/Interactable.h"
#include "../Data/ItemDataAsset.h"
#include "ItemActor.generated.h"

class USphereComponent;


UCLASS()
class BATTLESKY_API AItemActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AItemActor();
	void InitItem(UItemDataAsset* InData, int32 InQuantity);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	UItemDataAsset* ItemData;
	UPROPERTY(EditDefaultsOnly, Replicated)
	int32 Quantity = 1;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionSphere;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "Trace Settings")
	float InteractSphereRadius = 10.f;

public:
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE virtual bool TryInteract() override { return bCanInteract; };
	FORCEINLINE virtual FText GetText() const override;
	virtual AActor* Interact() override;
	
	/*virtual void EnableInteraction(bool bEnable) override;*/
private:
	bool bCanInteract = true;
};
