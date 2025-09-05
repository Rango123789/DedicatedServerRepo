// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/SignIn/UW_SignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void UUW_SignUpPage::NativeConstruct()
{
	Super::NativeConstruct();

	//disable the button by default, you can do it here or in WBP_SignUpPage - both fine!
	Button_SignUp->SetIsEnabled(false);
	
	EditableTextBox_UserName->OnTextChanged.AddDynamic(this, &ThisClass::OnTextChangedSharedCallback);
	EditableTextBox_Email->OnTextChanged.AddDynamic(this, &ThisClass::OnTextChangedSharedCallback);
	EditableTextBox_Password->OnTextChanged.AddDynamic(this, &ThisClass::OnTextChangedSharedCallback);
	EditableTextBox_ConfirmPassword->OnTextChanged.AddDynamic(this, &ThisClass::OnTextChangedSharedCallback);
	
}

void UUW_SignUpPage::UpdateStatusMessage(const FString& StatusMessage, bool bResetWidgetButtons)
{
	TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));

	if(bResetWidgetButtons)
	{
		Button_SignUp->SetIsEnabled(true);
	}
}

void UUW_SignUpPage::ClearTextBoxes() const
{
	//UText__::SetText("") and SetText(FText("")) NOT valid , SetText(FText::FromString("")); is valid but lengthy
	EditableTextBox_UserName->SetText(FText::GetEmpty()); 
	EditableTextBox_Email->SetText(FText::GetEmpty());
	EditableTextBox_Password->SetText(FText::GetEmpty());
	EditableTextBox_ConfirmPassword->SetText(FText::GetEmpty());

	//also clear TextBlock_StatusMessage too:
	TextBlock_StatusMessage->SetText(FText::GetEmpty());
}


/*
+OPTION1: You bind all EditableTextBox to the same callback to handle all at once
, I don't think this very good for performance.
=I don't consider this is professional at all, users may not like this

+OPTION2: You bind each EditableTextBox to a separate callback
, each EditableTextBox field will have its separate promting/status message (UTextBlock) adding right below each one in WBP_X
, we create each bValid_i member for each EditableTextBox field
, and we only enable the KEY button (say submit/signup button) when all bValid_i are true
, and we check if( && &&) at the end of all callbacks! yeah!
=this is perfect, users surely like this.

@@NOTE: in this lesson since I don't have time, so I follow OPTION1. However in future you can re-design the SignUpPage by inserting TextBLock_PromptingMessage1,2,3 + bValid1,2,3 members
*/
void UUW_SignUpPage::OnTextChangedSharedCallback(const FText& NewText)
{
	bool bIsValidUsername = EditableTextBox_UserName->GetText().ToString().Len() > 0;

	bool bIsValidEmail = IsValidEmail();

	//bool bIsValidPassword = IsValidPassword();
	
	bool bIsValidConfirmPassword =EditableTextBox_ConfirmPassword->GetText().ToString() == EditableTextBox_Password->GetText().ToString(); //FName has "==", but FText has NOT.
	
	FString StatusMessage = TEXT("");
	if (!bIsValidUsername)
	{
		StatusMessage = TEXT("Username can NOT be empty!");
	}
	else if (!bIsValidEmail)
	{
		StatusMessage = TEXT("Email is NOT valid!");
	}
	else if (!IsValidPassword(StatusMessage)) //UPDATE: you must directly call here for it to update, it is override when called too soon
	{
		//the status message will be changed INTERNALLY in 'IsValidPassword()' itself. Even if you do nothing, it is till important in the chain!
	}
	else if (!bIsValidConfirmPassword) 
	{
		StatusMessage = TEXT("Confirm Password does NOT match!");
	}
	else
	{
		StatusMessage = TEXT("");
		Button_SignUp->SetIsEnabled(true);
	}
	TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));
}

/*you can have "FString Email" param like stephen did(may be because you want to re-use it), but we don't spare param that we can always access it lol
you can always and add params later if you want*/
bool UUW_SignUpPage::IsValidEmail()
{
	FString Email = EditableTextBox_Email->GetText().ToString();

	//it has a constructor accept "FString SourceString"; OPTIONALLY TEXT(R"(....)")
		//my idea:      R"(^   [^@\s]+  @  ([^@\.\s]+ \. [^@\.\s]{2,})+    $)"
	    //              R"(^   [^@\s]+  @  ([^@\.\s]+ \.)+  [^@\.\s]{2,}   $)" --perfect!
	    //stephen idea: R"(^   [^@\s]+  @   [^@\s]+   \.    [^@\s]{2,}     $)"
	FRegexPattern Pattern(TEXT(R"(^[^@\s]+@([^@\.\s]+\.)+[^@\.\s]{2,}$)")); 
	FRegexMatcher Matcher(Pattern, Email);
	return Matcher.FindNext();
}

/*you can have "FString Password, "FString ConfirmPassword, &StatusMessage" param
, but we don't need params that we can always access it lol*/
bool UUW_SignUpPage::IsValidPassword(FString&  OutStatusMessage)
{
	FString Password = EditableTextBox_Password->GetText().ToString();

	//there is no need of "+", as I said it ONLY require any part of the input to match the pattern, the REDUNDANT number of characters doesn't matter, but when it smells there is first match character in the pattern all the rest must be STRICTLY match the next char of the pattern
	//you must NOT add "^" nor "$" because it will force the first letter of input to be on the check, without it any character of input can be used to check until it find match one (if any):
	FRegexPattern Pattern_OneUpperCase(TEXT(R"([A-Z])")); //INCCORRECT (^[A-Z]$)
	FRegexPattern Pattern_OneLowerCase(TEXT(R"([a-z])"));
	FRegexPattern Pattern_OneNumber(TEXT(R"(\d)")); //R"(^[0-9]$)"
	FRegexPattern Pattern_OneSpecialChar(TEXT(R"([^\w\s])")); //if U want special char to be at begin: (R"(^[^\w\s])")

	FRegexMatcher Matcher_OneUpperCase(Pattern_OneUpperCase, Password);
	FRegexMatcher Matcher_OneLowerCase(Pattern_OneLowerCase, Password);
	FRegexMatcher Matcher_OneNumber(Pattern_OneNumber, Password);
	FRegexMatcher Matcher_OneSpecialChar(Pattern_OneSpecialChar, Password);

	FString StatusMessage = TEXT("Password must contain at least: ");

	bool bIsValidPassword = true;
	if (Matcher_OneUpperCase.FindNext() == false)
	{
		StatusMessage+=TEXT("1 UpperCase, ");
		bIsValidPassword = false;
	}
	if (Matcher_OneLowerCase.FindNext() == false)
	{
		StatusMessage+=TEXT("1 LowerCase, ");
		bIsValidPassword = false;
	}
	if (Matcher_OneNumber.FindNext() == false)
	{
		StatusMessage+=TEXT("1 Number, ");
		bIsValidPassword = false;
	}
	if (Matcher_OneSpecialChar.FindNext() == false)
	{
		StatusMessage+=TEXT("1 SpecialChar, ");
		bIsValidPassword = false;
	}
	if (Password.Len() < 8)
	{
		StatusMessage+=TEXT("8 letters ");
		bIsValidPassword = false;
	}
	//how to trim ", " in case it is at the end of the StatusMessage here:
	//why length-2, because we start from "next final - final"! yeah! (the final index is "leng-1")
	if (StatusMessage.EndsWith(TEXT(", ")) )
	{
		StatusMessage.RemoveAt(StatusMessage.Len() - 2, 2);
	}
	
	//only change our OutStatusMessage when bIsValidPassword=false after all
	if (bIsValidPassword == false)
	{
		//TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));
		OutStatusMessage = StatusMessage;
		return false;
	}
	
	return true;
}