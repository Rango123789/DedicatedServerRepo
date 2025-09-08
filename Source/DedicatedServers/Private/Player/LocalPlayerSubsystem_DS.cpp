// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/LocalPlayerSubsystem_DS.h"
#include "UI/RequestManager/RequestManager_Portal.h"

//this function trigger after the first broadcast, so don't worry! 
//next time it won't trigger this any more  as i replace "SignInRequestSucceed.broadcast" already (WBP_Overlay::callback won't be called and won't trigger this any more), only trigger the NEW delegate callback!
void ULocalPlayerSubsystem_DS::SetAuthenticationResultAndPortalManager(
	const FAuthenticationResult& InAuthenticationResult,
	const TScriptInterface<IPortalInterface>& InPortalInterface, const FString& InLastUsername)
{
	if (IsValid(InPortalInterface.GetObject()) == false) return;
	AuthenticationResult = InAuthenticationResult;
		//RequestManager_Portal = InPortalInterface;
	PortalInterface = InPortalInterface;
	LastUsername = InLastUsername;

	//first time: we actively call it
	SetTimer_RefreshTokens();

	//next time when we broadcast from portal, this callback will be executed (this is just a binding step)
	//InRequestManager_Portal->RefreshTokensRequestSucceedDelegate.AddDynamic(this, &ThisClass::OnRefreshTokensRequestSucceed);
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
	if (IsValid(PortalInterface.GetObject()) == false) return;
//calling the SendRequest_RefreshTokens after every 3/4 hours (2700 sections):
	FTimerDelegate RefreshTokensDelegate;
	RefreshTokensDelegate.BindLambda([this]()
	{
		//you still need to recheck it, because is only executed after a delay lol :D :D
		if (IsValid(PortalInterface.GetObject()))
		{
			PortalInterface->SendRequest_RefreshTokens( LastUsername, AuthenticationResult.RefreshToken); //refresh, not access lol		
		}
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RefreshTokensDelegate, RefreshInterval, false);
}
