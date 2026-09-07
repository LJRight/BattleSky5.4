// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "ItemSystem/Actor/WeaponActor.h"
#include "Net/UnrealNetwork.h"

#include "BattleSkyPlayerController.h"
#include "BattleSkyCharacter.h"

#define DEBUG_MSG(Text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, Text);


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	WeaponSlots.SetNum((int32)EWeaponSlotType::None);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, InventoryItems);
	DOREPLIFETIME(UInventoryComponent, MaxWeight);
	DOREPLIFETIME(UInventoryComponent, CurrentWeight);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	//OwningCharacter = Cast<ABattleSkyCharacter>(GetOwner());
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

// 인벤토리에 넣기 로직 시작점
bool UInventoryComponent::AddItem(AItemActor* ItemActor)
{
	if (!ItemActor)
	{
		return false;
	}

	UItemDataAsset* Data = ItemActor->ItemData;
	int32 Quantity = ItemActor->Quantity;

	if (!Data || Quantity <= 0 || Data->Weight <= 0)
	{
		return false;
	}

	// 여기서 분기 크게는 리스트에 다 들어가는 아이템, 총기류, 방어구류
	if (AWeaponActor* Weapon = Cast<AWeaponActor>(ItemActor))
	{
		EquipWeapon(Weapon);
		return false;
	}
	else
	{
		// 스택형 아이템이 아닐 경우 (ex 파츠 종류)
		if (!Data->bStackable)
		{
			// 아이템 무게 추가 시 최대 소지 가능 무게보다 적다면 줍는다
			if (CanPickup(Data->Weight))
			{
				InventoryItems.Add(FInventoryItemData(Data, 1, FGuid::NewGuid()));
				InventoryItems.Sort();
				Quantity = 0;
				CurrentWeight += Data->Weight;

				// 여기서 이벤트 발행으로 UI 에 알리기 예정
				if (OnInventoryUpdated.IsBound())
				{

					OnInventoryUpdated.Broadcast(InventoryItems, CurrentWeight / MaxWeight);
				}

				ItemActor->Destroy();

				return true;
			}
			// 무게 때문에 주울 수 없다면 이벤트 발행으로 UI 로 표시 (ex "더 이상 주울 수 없습니다.")
			else
			{
				return false;
			}
		}
		// 스택형 아이템의 경우
		else
		{
			// 최대 주울 수 있는 개수 (가방 무게 제한 vs 들고자 하는 총 개수 중 작은값)
			int32 MaxAcceptQuantity = FMath::Min((MaxWeight - CurrentWeight) / Data->Weight, Quantity);
			int32 AddAmount = 0;
			// 주울 수 없을 경우 마찬가지로 이벤트 발행
			if (MaxAcceptQuantity <= 0)
			{
				// UI용 이벤트 발행
				return false;
			}
			// 기존 아이템 배열에서 추가하고자 하는 아이템 정적 데이터와 같은 것이 있다면 (= 스택형 아이템을 채울 것이 있다면)
			if (FInventoryItemData* ExistingItem = InventoryItems.FindByPredicate([Data](const FInventoryItemData& Item) { return Item.Data == Data;}))
			{
				int32 Space = Data->MaxStack - ExistingItem->Count;
				if (Space > 0)
				{
					AddAmount = FMath::Min(Space, MaxAcceptQuantity);
					ExistingItem->Count += AddAmount;
					MaxAcceptQuantity -= AddAmount;
					// ItemActor->Quantity -= AddAmount;

					CurrentWeight += AddAmount * Data->Weight;
				}
			}
			if (MaxAcceptQuantity > 0)
			{
				InventoryItems.Add(FInventoryItemData(Data, MaxAcceptQuantity, FGuid::NewGuid()));
				InventoryItems.Sort();

				AddAmount += MaxAcceptQuantity;
				ItemActor->Quantity -= AddAmount;
				CurrentWeight += MaxAcceptQuantity * Data->Weight;
			}
			if (ItemActor->Quantity == 0)
			{
				ItemActor->Destroy();
			}
			// 여기서 이벤트 발행으로 UI 에 알리기 예정
			if (OnInventoryUpdated.IsBound())
			{
				OnInventoryUpdated.Broadcast(InventoryItems, CurrentWeight / MaxWeight);
			}
			return true;
		}
	}
}

bool UInventoryComponent::DropItem(const FGuid TargetID)
{

	return false;
}


void UInventoryComponent::EquipWeapon(AWeaponActor* WeaponActor)
{
	if (!WeaponActor)
	{
		return;
	}
	UWeaponDataAsset* Data = Cast<UWeaponDataAsset>(WeaponActor->ItemData);
	FInventoryWeaponData CurrentWeaponData = FInventoryWeaponData(Data, WeaponActor->CurrentAmmo, WeaponActor->EquippedAttachments);
	switch (Data->WeaponType)
	{
	case EWeaponType::Pistol:
		if (!IsWeaponSlotEmpty((int32)EWeaponSlotType::Sidearm))
		{
			DropWeapon(WeaponSlots[(int32)EWeaponSlotType::Sidearm]);
		}
		WeaponSlots[(int32)EWeaponSlotType::Sidearm] = CurrentWeaponData;
		WeaponActor->Destroy();
		break;

	case EWeaponType::Melee:
		// 근접 무기는 나중에
		break;
	default :
		// 주무장 부무장 경우
		if (IsWeaponSlotEmpty((int32)EWeaponSlotType::Primary))
		{
			WeaponSlots[(int32)EWeaponSlotType::Primary] = CurrentWeaponData;
			WeaponActor->Destroy();
		}
		else if (IsWeaponSlotEmpty((int32)EWeaponSlotType::Secondary))
		{
			WeaponSlots[(int32)EWeaponSlotType::Secondary] = CurrentWeaponData;
			WeaponActor->Destroy();
		}
		else
		{
			DropWeapon(WeaponSlots[(int32)EWeaponSlotType::Secondary]);
			WeaponSlots[(int32)EWeaponSlotType::Secondary] = CurrentWeaponData;
			WeaponActor->Destroy();
		}
		break;
	}
	if (OnWeaponSlotUpdated.IsBound())
	{
		OnWeaponSlotUpdated.Broadcast(WeaponSlots);
	}
}

void UInventoryComponent::DropWeapon(const FInventoryWeaponData& DropTarget)
{
	// GetWorld()->SpawnActor()
}

//bool UInventoryComponent::EquipItem(AItemActor* Item)
//{
//	if (!Item)
//	{
//		return false;
//	}
//	if (AWeaponBase* Weapon = Cast<AWeaponBase>(Item))
//	{
//		//return EquipWeapon(Weapon);
//	}
//	return false;
//}


//bool UInventoryComponent::EquipWeapon(AWeaponBase* TargetWeapon)
//{
//	if (!TargetWeapon)
//	{
//		return false;
//	}
//
//    
//    switch (TargetWeapon->WeaponType)
//    {
//    case EWeaponType::Pistol:
//        if (!WeaponSlots[(int32)EWeaponSlot::Sidearm])
//        {
//            WeaponSlots[(int32)EWeaponSlot::Sidearm] = TargetWeapon;
//			// OutSlotIndex = (int32)EWeaponSlot::Sidearm;
//            return true;
//        }
//        break;
//    
//    case EWeaponType::Melee:
//        if (!WeaponSlots[(int32)EWeaponSlot::Melee])
//        {
// 
//            WeaponSlots[(int32)EWeaponSlot::Melee] = TargetWeapon;
//            //OutSlotIndex = (int32)EWeaponSlot::Melee;
//            return true;
//        }
//        break;
//
//    default:
//        // AR / SR / DMR / SMG / Shotgun
//
//        // 주요 로직. 주무기 칸 확인 -> 비어있으면 해당 슬롯에 장착, 
//        // 비어있지 않다면 부무기 확인 -> 비어있다면 해당 슬롯에 장착
//        // 부무기까지 장착되어 있으면, 현재 주무기와 교체
//
//        AItemActor* PrimarySlotWeapon = GetWeapon((int32)EWeaponSlot::Primary);
//        AItemActor* SecondarySlotWeapon = GetWeapon((int32)EWeaponSlot::Secondary);
//        AItemActor* CurrentWeapon = OwningCharacter->Replicated_CurrentEquipedWeapon;
//
//		bool bHasPrimary = PrimarySlotWeapon != nullptr;
//		bool bHasSecondary = SecondarySlotWeapon != nullptr;
//		bool bIsHoldingWeapon = CurrentWeapon != nullptr;
//
//        // 첫 번째 슬롯이 비어있을 때
//        if (!bHasPrimary)
//        {
//		    // 슬롯에 넣기
//			WeaponSlots[(int32)EWeaponSlot::Primary] = TargetWeapon;
//            // 무기를 들고 있다면
//            if (bIsHoldingWeapon)
//            {
//                // 몸에 붙이기
//				OwningCharacter->AttachToBody(TargetWeapon, EWeaponSlot::Primary);
//			}
//            // 안 들고 있다면
//            else
//            {
//                // 손으로 들기
//                OwningCharacter->HoldWeapon(TargetWeapon);
//            }
//			return true;
//        }
//        // 첫 번째 슬롯은 있고, 두 번째 슬롯이 비어있을 때
//        if (!bHasSecondary)
//        {
//            // 슬롯에 넣고
//			WeaponSlots[(int32)EWeaponSlot::Secondary] = TargetWeapon;
//            // 무기를 들고 있든 아니든
//			// 몸에 붙이기
//			OwningCharacter->AttachToBody(TargetWeapon, EWeaponSlot::Secondary);
//			return true;
//        }
//        // 두 슬롯 모두 있을 때,
//        else
//        {
//			OwningCharacter->DropItem(SecondarySlotWeapon);
//            WeaponSlots[(int32)EWeaponSlot::Secondary] = TargetWeapon;
//            OwningCharacter->AttachToBody(TargetWeapon, EWeaponSlot::Secondary);
//			return true;
//    //        // 무기를 들고 있다면,
//    //        if (bIsHoldingWeapon)
//    //        {
//    //            // 들고 있는 것을 바닥에 내려놓고, 무기 장착
//				//OwningCharacter->DropItem(CurrentWeapon);
//				//OwningCharacter->HoldWeapon(TargetWeapon);
//    //        }
//    //        // 무기를 안 들고 있으면,
//    //        else
//    //        {
//    //            // 부무장을 바닥에 버리고, 등에 부착
//				//OwningCharacter->DropItem(SecondarySlotWeapon);
//				//WeaponSlots[(int32)EWeaponSlot::Secondary] = TargetWeapon;
//				//OwningCharacter->AttachToBody(TargetWeapon, EWeaponSlot::Secondary);
//    //        }
//        }
//        
//
//		/*if (AWeaponBase* CurrentPrimary = GetWeapon((int32)EWeaponSlot::Primary))
//        {
//            if (AWeaponBase* CurrentSecondary = GetWeapon((int32)EWeaponSlot::Secondary))
//            {
//                if (OwningCharacter)
//                {
//					OwningCharacter->DropItem(CurrentPrimary);
//					WeaponSlots[(int32)EWeaponSlot::Primary] = TargetWeapon;
//                    OwningCharacter->PickupItem(TargetWeapon, );
//                }
//            }
//            WeaponSlots[(int32)EWeaponSlot::Primary] = Weapon;
//            OutSlotIndex = (int32)EWeaponSlot::Primary;
//            return true;
//        }
//        else if (!WeaponSlots[])
//        {
//            WeaponSlots[(int32)EWeaponSlot::Secondary] = Weapon;
//			OutSlotIndex = (int32)EWeaponSlot::Secondary;
//            return true;
//        }*/
//        break;
//	}
//    return false;
//}
