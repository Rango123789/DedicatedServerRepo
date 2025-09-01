// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_ConfirmSignUpPage.generated.h"
class UTextBlock;
class UEditableTextBox;
class UButton;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_ConfirmSignUpPage : public UUserWidget
{
	GENERATED_BODY()
public:
//text:
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_VerificationCode;

	//this one is NOT user input, so UTextBlock
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessage;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Destination; //***email**
	
//buttons
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_Confirm;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_Back;
};
