// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestAPIOverlay.generated.h"

class UUW_ListFleetsBox;
class URequestManager_TestAPIOverlay;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UTestAPIOverlay : public UUserWidget
{
	GENERATED_BODY()
public:

protected:
	virtual void NativeConstruct() override;

	//because it is bound, so you need to access and use it (WBP_HOST is the one need to add this WBP_X from BP)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUW_ListFleetsBox> WBP_ListFleetsBox; //name WBP_X exactly like this in WBP_Host
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<URequestManager_TestAPIOverlay> RequestManager_TestAPIOverlay_Class;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<URequestManager_TestAPIOverlay> RequestManager_TestAPIOverlay;

public:
	
};
