// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UI/RequestManager/HTTPRequestTypes.h"
#include "LocalPlayerSubsystem_DS.generated.h"

class IPortalInterface;
class URequestManager_Portal;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ULocalPlayerSubsystem_DS : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	//they're X_SignIn:
	UPROPERTY()
	FString Username;
	UPROPERTY()
	FString Email;
	

	FTimerHandle TimerHandle;

	//this UPROPERTY() is very important that help Portal to alive even if its Outer "WBP_SignInOverlay" get destroyed (either because we close it down or we move to new level). Stephen mentioned something like this.
		//UPROPERTY()
		//TObjectPtr<URequestManager_Portal> RequestManager_Portal;
	UPROPERTY()
	TScriptInterface<IPortalInterface> PortalInterface;
	
	//you may not see this anywhere to edit it lol, but anyway lol
	UPROPERTY(EditAnywhere)
	float RefreshInterval = 3200.f; //current 75% of expiration time of Access,Id tokens = 3/4 hour

	void SetTimer_RefreshTokens();
	void CacheDataToSubsystem(const FAuthenticationResult& InAuthenticationResult, const TScriptInterface<IPortalInterface>& InPortalInterface, const FString& InUsername, const
	                          FString& InEmail);
	
	UFUNCTION()
	void OnRefreshTokensRequestSucceed(const FString& InUsername, const FAuthenticationResult& InAuthenticationResult);
	UFUNCTION()
	void OnSignOutRequestSucceed();

protected:
	/*the trick to NOT let external classes to get data member of a class but can NOT modify it (even if the class itself can modify it)
	+declare the data in protected/private session
	+create "const getter" - so yeah you can't not modify it from wherever you get it, you could only read it :D :D*/
	//no need "FDSSignIn LastDSSignIn", we only need FDSSignIn::FAuthenticationResult, this is where all tokens we need are in:
	UPROPERTY()
	FAuthenticationResult AuthenticationResult;

public:
	const FString& GetAccessToken(){return AuthenticationResult.AccessToken; }

	//this is optional PART4: direclty parse email+ from IdToken:
	FString Base64UrlDecode(const FString& Input);
	FString GetEmailFromIdToken(const FString& IdToken);
};
