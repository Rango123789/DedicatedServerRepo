// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SignInPage.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_SignInPage : public UUserWidget
{
	GENERATED_BODY()

public:
//text: we need to use "UEditableTextBox" (not UTextBlock, not UEditableTextBlock - not exist). you may want to name it "TextBox" but I prefer to use EditableTextBox, so that it can be mistaken with "TextBlock" lol!
	//warning: its display type is "Text Box" in WBP Editor (noy "Editable Text" - that is also for user input but didn't have "VISUAL box"! yeah!)
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_UserName;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_Password;

	//this one is NOT user input, so UTextBlock
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessage;
	
//buttons:
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_SignIn;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_SignUp;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_Quit;
	UFUNCTION()
	void UpdateStatusMessage(const FString& StatusMessage, bool bResetWidgetButtons);
};
