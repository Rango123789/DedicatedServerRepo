// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_TestRecordMatchStats.generated.h"

class UButton;
class URequestManager_GameStats;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_TestRecordMatchStats : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void OnRecordMatchStatsButtonClicked();
	virtual void NativeConstruct() override;
	
	UPROPERTY()
	TObjectPtr<URequestManager_GameStats> RequestManager_GameStats;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<URequestManager_GameStats> RequestManager_GameStats_Class;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_RecordMatchStats;
};
