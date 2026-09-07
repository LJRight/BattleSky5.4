#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemTypes.h"
#include "ItemDataAsset.generated.h"

class UTexture2D;
class UStaticMesh;

// 기본 아이템 데이터 정보를 담는 클래스 (데이터와 로직의 분리 목적, 정적인 데이터)
UCLASS()
class BATTLESKY_API UItemDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName ItemID;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UStaticMesh* WorldMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bStackable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxStack;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemType Type;
};