// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSkyGameInstance.h"
#include "UIManagerSubsystem.h"
#include "LANSessionSubsystem.h"
void UBattleSkyGameInstance::Init()
{
	Super::Init();

	SetUIWidget();
}

void UBattleSkyGameInstance::OnStart()
{
	Super::OnStart();

	GetSubsystem<UUIManagerSubsystem>()->Subscribe();
	GetSubsystem<ULANSessionSubsystem>()->Subscribe();
}
