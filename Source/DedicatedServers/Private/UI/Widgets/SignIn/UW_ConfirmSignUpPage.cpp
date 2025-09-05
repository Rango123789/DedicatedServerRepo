// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/SignIn/UW_ConfirmSignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void UUW_ConfirmSignUpPage::UpdateStatusMessage(const FString& StatusMessage, bool bResetWidgetButtons)
{
	TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));

	//why must enable it back? if so should disable it earlier (yes we did disable all buttons in OnXButtonClicked even before SendRequest_X is made lol! so don't worry about it any more! it's all setup in early lesson)
	//perhaps leave it disabled until enter the right pattern [\n]+ right? well who knows some services allow non-number characters as well? and yes we did NOT enforce any restriction in WBP_3 at all so yeah lol. but indeed you should - maybe in future lol!
	if(bResetWidgetButtons)
	{
		Button_Confirm->SetIsEnabled(true);
	}
}

void UUW_ConfirmSignUpPage::ClearTextBoxes()
{
	//this is the only box it has:
	EditableTextBox_VerificationCode->SetText(FText::GetEmpty());
	//also clear TextBlock_StatusMessage and TextBlock_Destination too
	TextBlock_StatusMessage->SetText(FText::GetEmpty());
	TextBlock_Destination->SetText(FText::GetEmpty());
}


void UUW_ConfirmSignUpPage::NativeConstruct()
{
	Super::NativeConstruct();

	//disable the KEY button:
	Button_Confirm->SetIsEnabled(false);

	//bind callback:
	EditableTextBox_VerificationCode->OnTextChanged.AddDynamic(this, &ThisClass::OnVerifimationCodeTextChanged);
}

void UUW_ConfirmSignUpPage::OnVerifimationCodeTextChanged(const FText& NewText)
{
	//there is only one EditableTextBox_X this time, so this step is REDUNDANT lol, you can simply use "NextText"
	FText ConfirmationCode = EditableTextBox_VerificationCode->GetText();

	//again: if you don't add ^__$ , players can enter xyz123456abc and pass the check lol: 
	FRegexPattern Pattern(TEXT(R"(^\d{6,6}$)")); //or simply TEXT(R"(\d{6})")
	FRegexMatcher Matcher(Pattern, ConfirmationCode.ToString());

	FString StatusMessage = TEXT("");
	if (Matcher.FindNext())
	{
		Button_Confirm->SetIsEnabled(true);
		StatusMessage = TEXT("");
	}
	else
	{
		StatusMessage = TEXT("Please enter 6 digits!");
	}

	TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));
}

