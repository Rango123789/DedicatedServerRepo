// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/RequestManager/HTTPRequestTypes.h"
#include "TestAPIOverlay.generated.h"

class UUW_FleetId;
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
	
	UFUNCTION()
	void OnListFleetsResponseReceived(const FDSListFleetsResponse& DSListFleetsReponse, bool bWasSuccessful);

	UFUNCTION()
	void OnClickedCallback_ListFleets();

	//because it is bound, so you need to access and use it (WBP_HOST is the one need to add this WBP_X from BP)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUW_ListFleetsBox> WBP_ListFleetsBox; //name WBP_X exactly like this in WBP_Host

	/*My conventions:
	+I always name TSubclassOf<U/A__> ___ (NOT upgrade it into BP_ , WBP_ ...)
	+but I may name the bindwidget name like this "WBP_X" if it is a whole WBP_X! Reason: the default name when we drag WBP_X into WBP_Host from WBP Editor is [WBP_X]! so that you only need to check "IsVariable" and the name match! 
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUW_FleetId> UW_FleetId_Class;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<URequestManager_TestAPIOverlay> RequestManager_TestAPIOverlay_Class;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<URequestManager_TestAPIOverlay> RequestManager_TestAPIOverlay;

public:
	
};
