// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SuccessConfirmSignUpPage.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_SuccessConfirmSignUpPage : public UUserWidget
{
	GENERATED_BODY()
public:
//buttons: we're not going to this page if it fails, so no need StatusMessage or whatsoever:
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_OK;
};
