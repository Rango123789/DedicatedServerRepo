// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_JoinGame.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_JoinGame : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UButton> Button_JoinGame;
	
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UTextBlock> TextBlock_StatusMessage;
	
	void SetStatusMessage(const FString& InStatusMessage) const;
protected:
	
};
