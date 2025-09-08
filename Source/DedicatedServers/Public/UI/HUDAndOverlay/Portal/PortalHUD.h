// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PortalHUD.generated.h"

class UUW_DashboardOverlay;
class UUW_SigninOverlay;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API APortalHUD : public AHUD
{
	GENERATED_BODY()

public:
	void PostSignIn();
	
protected:
	virtual void BeginPlay() override;
	

	/*@@IMPORTANT notes:
	(1) Manager is added to WBP_HUD help it handle side works about HTTP requesting and receiving
	, not added to HUD in this course lol!
	(2) in GAS course we did create all WC_X,Y,Z and add them all to HUD level
	, so that we make it possible they can be accessible anywhere if needed (not the case in this course so yeah)*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUW_SigninOverlay> UW_SigninOverlay_Class;
	UPROPERTY()
	TObjectPtr<UUW_SigninOverlay> UW_SigninOverlay;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUW_DashboardOverlay> UW_DashboardOverlay_Class;
	UPROPERTY()
	TObjectPtr<UUW_DashboardOverlay> UW_DashboardOverlay;

public:
	
};
