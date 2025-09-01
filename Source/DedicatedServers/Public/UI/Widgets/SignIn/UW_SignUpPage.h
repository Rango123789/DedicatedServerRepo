// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SignUpPage.generated.h"
class UTextBlock;
class UEditableTextBox;
class UButton;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_SignUpPage : public UUserWidget
{
	GENERATED_BODY()
public:
//text:
	//these are for User inputs, so UEditableTextBox
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_UserName;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_Password;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ConfirmPassword;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_Email;

	//this one is NOT user input, so UTextBlock
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessage;
	

	//buttons
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_SignUp;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_Back;

protected:
	bool IsValidEmail();
	bool IsValidPassword(FString& OutStatusMessage);
	//you don't really need the NewText because you can always access it from EditableText_i variable lol
	UFUNCTION()
	void OnTextChangedSharedCallback(const FText& NewText);
	
	virtual void NativeConstruct() override;
};
