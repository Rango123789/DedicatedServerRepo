// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UI/RequestManager/HTTPRequestTypes.h"
#include "LocalPlayerSubsystem_DS.generated.h"

class URequestManager_Portal;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ULocalPlayerSubsystem_DS : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	//no need "FDSSignIn LastDSSignIn", we only need FDSSignIn::FAuthenticationResult, this is where all tokens we need are in:
	UPROPERTY()
	FAuthenticationResult AuthenticationResult;
	UPROPERTY()
	FString LastUsername;

	FTimerHandle TimerHandle;

	//this UPROPERTY() is very important that help Portal to alive even if its Outer "WBP_SignInOverlay" get destroyed (either because we close it down or we move to new level). Stephen mentioned something like this.
	UPROPERTY()
	TObjectPtr<URequestManager_Portal> RequestManager_Portal;

	//you may not see this anywhere to edit it lol, but anyway lol
	UPROPERTY(EditAnywhere)
	float RefreshInterval = 2700.f; //current 75% of expiration time of Access,Id tokens = 3/4 hour

	void SetTimer_RefreshTokens();
	void SetAuthenticationResultAndPortalManager(const FAuthenticationResult& InAuthenticationResult, const TObjectPtr<URequestManager_Portal>& InRequestManager_Portal);
	
	UFUNCTION()
	void OnRefreshTokensRequestSucceed(const FString& InUsername, const FAuthenticationResult& InAuthenticationResult);
};
