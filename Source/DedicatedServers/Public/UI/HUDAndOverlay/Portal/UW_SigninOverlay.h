// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/RequestManager/HTTPRequestTypes.h"
#include "UW_SigninOverlay.generated.h"

class UUW_JoinGame;
class URequestManager_Portal;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_SigninOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	
protected:
	UFUNCTION()
	void OnStatusMessageDelegateBroadcast(const FString& StatusMessage, bool bResetJoinGameButton);
	UFUNCTION()
	void OnJoinGameResponseReceived(const FDSGameSession& DSGameSession, bool bWasSuccessful);
	
	UFUNCTION()
	void OnJoinGameButtonClicked();
	virtual void NativeConstruct() override;
	
	/*@@IMPORTANT notes:
     	(1) Manager is added to WBP_HUD help it handle side works about HTTP requesting and receiving
     	, not added to HUD in this course lol!
     	(2) in GAS course we did create all WC_X,Y,Z and add them all to HUD level
     	, so that we make it possible they can be accessible anywhere if needed (not the case in this course so yeah)*/
     UPROPERTY(EditAnywhere)
	TSubclassOf<URequestManager_Portal> RequestManager_Portal_Class;
	UPROPERTY()
	TObjectPtr<URequestManager_Portal> RequestManager_Portal;

	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UUW_JoinGame> WBP_JoinGame;
};
