// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/API/GameSessions/UW_JoinGame.h"

#include "Components/TextBlock.h"

void UUW_JoinGame::SetStatusMessage(const FString& InStatusMessage) const
{
	TextBlock_StatusMessage->SetText(FText::FromString(InStatusMessage));
}
