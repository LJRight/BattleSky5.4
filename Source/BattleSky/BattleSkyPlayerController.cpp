// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSkyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "BattleSkyCharacter.h"
#include "EnhancedInputComponent.h"
#include "BattleSkyCameraManager.h"
#include "UIManagerSubsystem.h"

#include "ItemSystem/Actor/ItemActor.h"
#include "InventoryComponent.h"


#define DEBUG_MSG(Text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, Text);

ABattleSkyPlayerController::ABattleSkyPlayerController()
{
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void ABattleSkyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
		if (!EIC) return;

		
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnMove);
		EIC->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnMouseLook);	

		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnJump);	
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnJumpEnd);
		
		// Toggle
		EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnCrouch);	
		EIC->BindAction(ProneAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnProne);

		EIC->BindAction(ViewModeAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnViewModeChanged);
		EIC->BindAction(WeapongChangeAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnWeaponChange);

		EIC->BindAction(InteractableAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnInteraction);

		EIC->BindAction(InventoryAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnInventory);

		// maintain
		EIC->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnWalk);
		EIC->BindAction(WalkAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnWalk);

		EIC->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnSprint);
		EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnSprint);

		EIC->BindAction(FreeLookAction, ETriggerEvent::Started, this, &ABattleSkyPlayerController::OnFreeLook);
		EIC->BindAction(FreeLookAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnFreeLook);

		EIC->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnFire);
		EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnStopFire);

		EIC->BindAction(AimingAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnAiming);
		EIC->BindAction(AimingAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnAiming);

		EIC->BindAction(PeekingAction, ETriggerEvent::Triggered, this, &ABattleSkyPlayerController::OnPeeking);
		EIC->BindAction(PeekingAction, ETriggerEvent::Completed, this, &ABattleSkyPlayerController::OnPeeking);
		
	}
}

void ABattleSkyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
	UI = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
}

void ABattleSkyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bReturningFromFreeLook)
	{
		ReturnToFreeLookStartRotationByTime(DeltaTime);
	}

	if (ABattleSkyCharacter* BSCharacter = Cast<ABattleSkyCharacter>(GetCharacter()))
	{
		if (IsLocalPlayerController())
		{
			SearchInteractableObjects();
		}
	}
}


void ABattleSkyPlayerController::ReturnToFreeLookStartRotationByTime(float DeltaTime)
{
	FreeLookReturnElapsed += DeltaTime;

	float Alpha = FMath::Clamp(
		FreeLookReturnElapsed / FreeLookReturnDuration,
		0.f,
		1.f
	);
	Alpha = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.f);

	FQuat Start = FreeLookReturnStartRotation.Quaternion();
	FQuat Target = FreeLookReturnTargetRotation.Quaternion();

	FQuat Result = FQuat::Slerp(Start, Target, Alpha);

	SetControlRotation(Result.Rotator());
	if (Alpha >= 1.f)
	{
		bReturningFromFreeLook = false;
	}
}

void ABattleSkyPlayerController::OnMove(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>()) 
	{
		BSCharacter->DoMove(Value.Get<FVector2D>(), FRotator(0.f, IsFreeLooking ? FreeLookReturnTargetRotation.Yaw : ControlRotation.Yaw, 0.f));
	}
}

void ABattleSkyPlayerController::OnMouseLook(const FInputActionValue& Value)
{
	if (bReturningFromFreeLook)
	{
		return;
	}
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X * HorizontalLookRate);
	AddPitchInput(LookAxisVector.Y * VerticalLookRate);
}

void ABattleSkyPlayerController::OnJump(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoJump(Value);
	}
}

void ABattleSkyPlayerController::OnJumpEnd(const FInputActionValue& Value)
{
}

void ABattleSkyPlayerController::OnCrouch(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoCrouch(Value);
	}
}

void ABattleSkyPlayerController::OnProne(const FInputActionValue& Value)
{
}

void ABattleSkyPlayerController::OnWalk(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoWalk(Value);
	}
} 

void ABattleSkyPlayerController::OnSprint(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoSprint(Value);
	}
}

void ABattleSkyPlayerController::OnFreeLook(const FInputActionValue& Value)
{
	IsFreeLooking = Value.Get<bool>();
	if (IsFreeLooking)
	{
		FreeLookReturnTargetRotation = ControlRotation;
	}
	else
	{
		bReturningFromFreeLook = true;
		FreeLookReturnStartRotation = GetControlRotation();
		FreeLookReturnElapsed = 0.f;
	}
}

void ABattleSkyPlayerController::OnFire(const FInputActionValue& Value)
{
	/*if (Value.Get<bool>())
	{
		if (ABattleSkyCharacter* BSCharacter = Cast<ABattleSkyCharacter>(GetCharacter()))
		{
			FVector Start;
			FRotator Rotation;
			GetPlayerViewPoint(Start, Rotation);

			BSCharacter->DoFire(Start, Rotation);
		}
	}*/
}

void ABattleSkyPlayerController::OnStopFire(const FInputActionValue& Value)
{	
}

// 시점 변경 시
void ABattleSkyPlayerController::OnViewModeChanged(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->ChangeViewMode(Value);
	}
}

void ABattleSkyPlayerController::OnAiming(const FInputActionValue& Value)
{
	/*if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->OverlayState = Value.Get<bool>() ? EOverlayState::Rifle : EOverlayState::Default;
		BSCharacter->Replicated_RotationMode = Value.Get<bool>() ? ERotationMode::Aiming : ERotationMode::LookingDirection;
	}*/
}


void ABattleSkyPlayerController::OnPeeking(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoPeeking(Value.Get<float>());
	}
}

void ABattleSkyPlayerController::OnWeaponChange(const FInputActionValue& Value)
{
	if (ABattleSkyCharacter* BSCharacter = GetPawn<ABattleSkyCharacter>())
	{
		BSCharacter->DoChangeWeapon((int)Value.Get<float>());
	}
}

// 상호작용 가능한 물체에 대해서 로컬 입력으로 상호작용 수행
void ABattleSkyPlayerController::OnInteraction(const FInputActionValue& Value)
{
	if (FocusedInteractableActor)
	{
		Server_DoInteract(FocusedInteractableActor);
	}
}

// 줍기 담당 액션 최종 (F키, 드래그, 우클릭)
void ABattleSkyPlayerController::Server_DoInteract_Implementation(AActor* Target)
{
	IInteractable* Interactable = Cast<IInteractable>(Target);
	if (Interactable && Interactable->TryInteract())
	{
		Interactable->Interact();
		if (Inventory)
		{
			
		}
	}
}

void ABattleSkyPlayerController::PickupItem(AActor* ItemActor)
{
	if (Inventory)
	{
		/*if (Inventory->AddItem(InData, OutQuantity))
		{

		}*/
	}
}

void ABattleSkyPlayerController::HandleInventoryLogic(const FSlotInfo& Source, const FSlotInfo& Target, FGuid ItemID)
{

}

// 액터 포인터를 입력으로 받는 경우 항상 월드 상의 아이템에 대한 로직이다
void ABattleSkyPlayerController::HandleInventoryLogic(const FSlotInfo& Source, const FSlotInfo& Target, AActor* ItemActor)
{
	if (!ItemActor || Source.Group != ESlotGroup::World)
	{
		return;
	}
	switch (Target.Group)
	{
	case ESlotGroup::Inventory:
		break;
	case ESlotGroup::Weapon:
		break;
	case ESlotGroup::Equipment:
		break;
	case ESlotGroup::Attachment:
		break;
	}
}

void ABattleSkyPlayerController::OnInventory(const FInputActionValue& Value)
{
	InventoryOpenState = !InventoryOpenState;
	if (UI)
	{
		UI->ShowInventory(InventoryOpenState);
	}
	if (ABattleSkyCharacter* BSCharacter = Cast<ABattleSkyCharacter>(GetPawn()))
	{
		BSCharacter->SearchAround(InventoryOpenState);
	}

	if (InventoryOpenState)
	{
		bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
		SetIgnoreLookInput(true);
	}
	else
	{
		bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SetIgnoreLookInput(false);
	}
}

void ABattleSkyPlayerController::SearchInteractableObjects()
{
	FHitResult HitResult;
	
	FVector Start;
	FRotator Rotation;

	GetPlayerViewPoint(Start, Rotation);

	FVector End = Start + Rotation.Vector() * 500.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_GameTraceChannel2,
		Params
	);

	if (bHit)
	{
		AActor* NewActor = HitResult.GetActor();
		if (NewActor && NewActor != FocusedInteractableActor)
		{
			FocusedInteractableActor = NewActor;
			if(UI)
			{
				UI->ShowInteractWidget(
					true, 
					FText::Format(FText::FromString("[{0}] {1}"), GetInteractKey().GetDisplayName(), Cast<IInteractable>(FocusedInteractableActor)->GetText()));
			}
		}
	}
	else
	{
		FocusedInteractableActor = nullptr;
		if (UI && UI->CanHideInteractionWidget())
		{
			UI->ShowInteractWidget(false, FText::GetEmpty());
		}
	}
}

FKey ABattleSkyPlayerController::GetInteractKey() const
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			TArray<FEnhancedActionKeyMapping> Mappings = Subsystem->GetAllPlayerMappableActionKeyMappings();

			for (const auto& Mapping : Mappings)
			{
				if (Mapping.Action == InteractableAction)
				{
					return Mapping.Key;
				}
			}
		}
	}
	return EKeys::Invalid;
}