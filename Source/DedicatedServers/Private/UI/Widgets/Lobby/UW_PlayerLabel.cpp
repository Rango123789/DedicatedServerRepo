// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Lobby/UW_PlayerLabel.h"

#include "Components/TextBlock.h"

void UUW_PlayerLabel::SetPlayerName(const FString& PlayerName) const
{
	TextBlock_UserName->SetText(FText::FromString(PlayerName));
}

FString UUW_PlayerLabel::GetPlayerName() const
{
	return TextBlock_UserName->GetText().ToString();
}
