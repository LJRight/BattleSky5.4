// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionLobbyWidget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"

void USessionLobbyWidget::UpdateLobbyPlayerNames(const TArray<FString>& PlayerNames)
{
	if (!PlayerListBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerListBox is null in UpdateLobbyPlayerNames"));
		return;
	}
	PlayerListBox->ClearChildren();

    for (const FString& Name : PlayerNames)
    {
        UTextBlock* PlayerText = NewObject<UTextBlock>(this);
        if (PlayerText)
        {
            PlayerText->SetText(FText::FromString(Name));
            
            // PlayerText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24)); // 원하는 폰트/사이즈

            // VerticalBox에 추가
            PlayerListBox->AddChild(PlayerText);
        }
    }
}