// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_PlayerLabel.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_PlayerLabel : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetPlayerName(const FString& PlayerName) const;
	FString GetPlayerName() const;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_UserName;
	
};
