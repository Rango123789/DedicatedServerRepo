// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/SignIn/UW_SignInPage.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UUW_SignInPage::UpdateStatusMessage(const FString& StatusMessage, bool bResetWidgetButtons)
{
	TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));

	if (bResetWidgetButtons)
	{
		Button_SignIn->SetIsEnabled(true);
	}
}
