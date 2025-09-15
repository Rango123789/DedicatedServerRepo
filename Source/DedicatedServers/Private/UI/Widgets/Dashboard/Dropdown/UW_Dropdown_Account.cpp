// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Dashboard/Dropdown/UW_Dropdown_Account.h"

#include "Components/TextBlock.h"
#include "Player/LocalPlayerSubsystem_DS.h"

void UUW_Dropdown_Account::NativeConstruct()
{
	Super::NativeConstruct();

	if (GetLocalPlayerSubsystem_DS())
	{
		TextBlock_ButtonText->SetText(FText::FromString(GetLocalPlayerSubsystem_DS()->Username));
	}
}
