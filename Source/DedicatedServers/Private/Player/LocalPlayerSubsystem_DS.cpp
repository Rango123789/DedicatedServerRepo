// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LocalPlayerSubsystem_DS.h"

#include "UI/RequestManager/RequestManager_Portal.h"

//this function trigger after the first broadcast, so don't worry! 
//next time it won't trigger this any more  as i replace "SignInRequestSucceed.broadcast" already (WBP_Overlay::callback won't be called and won't trigger this any more), only trigger the NEW delegate callback!
void ULocalPlayerSubsystem_DS::SetAuthenticationResultAndPortalManager(
	const FAuthenticationResult& InAuthenticationResult,
	const TObjectPtr<URequestManager_Portal>& InRequestManager_Portal)
{
	if (IsValid(InRequestManager_Portal) == false) return;
	AuthenticationResult = InAuthenticationResult;
	RequestManager_Portal = InRequestManager_Portal;
	LastUsername = InRequestManager_Portal->LastUsername_SignIn;

	//first time: we actively call it
	SetTimer_RefreshTokens();
	//next time when we broadcast from portal, callback will be executed (this is just binding step)
	InRequestManager_Portal->RefreshTokensRequestSucceedDelegate.AddDynamic(this, &ThisClass::OnRefreshTokensRequestSucceed);
}

void ULocalPlayerSubsystem_DS::OnRefreshTokensRequestSucceed(const FString& InUsername, const FAuthenticationResult& InAuthenticationResult)
{
	LastUsername = InUsername; //no need, it shouldn't be changed at all
	
	//better off... rather than AuthenticationResult = InAuthenticationResult; because RefreshToken+ isn't given in Refresh mode 
	AuthenticationResult.AccessToken = InAuthenticationResult.AccessToken;
	AuthenticationResult.IdToken = InAuthenticationResult.IdToken;
	SetTimer_RefreshTokens();
}

//only trigger when broadcast, hence we must call "SetTimer_RefreshTokens();" for th first time above

//"Username" for SECRET_HASH for refresh tokens should be in lower-case. but we handle it from lambda already, so no worry.  
void ULocalPlayerSubsystem_DS::SetTimer_RefreshTokens()
{
//calling the SendRequest_RefreshTokens after every 3/4 hours (2700 sections):
	FTimerDelegate RefreshTokensDelegate;
	RefreshTokensDelegate.BindLambda([this]()
	{
		RequestManager_Portal->SendRequest_RefreshTokens( LastUsername, AuthenticationResult.RefreshToken); //refresh, not access lol
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RefreshTokensDelegate, RefreshInterval, false);
}
