// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTPRequestTypes.h"
#include "RequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/PortalInterface.h"
#include "RequestManager_Portal.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusMessageDelegate, const FString& , StatusMessage, bool, bResetWidgetButtons);
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API URequestManager_Portal : public URequestManager, public IPortalInterface
{
	GENERATED_BODY()
public:
	
	/*UPROPERTY(BlueprintAssignable)
	FStatusMessageDelegate StatusMessageDelegate;
	
	void SendRequest_FindAndJoinGameSession();
	void OnResponse_CreatePlayerSession(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);
	
	void SendRequest_CreatePlayerSession(const FString& GameSessionId, const FString& PlayerId);
	void OnResponse_FindOrCreateGameSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleCreatePlayerSession(const FString& GameSessionId, const FString& Status);*/

	//status messages
	UPROPERTY(BlueprintAssignable)
	FStatusMessageDelegate SignUpStatusMessageDelegate;
	UPROPERTY(BlueprintAssignable)
	FStatusMessageDelegate ConfirmSignUpStatusMessageDelegate;
	UPROPERTY(BlueprintAssignable)
	FStatusMessageDelegate SignInStatusMessageDelegate;
	
	//succeed:
	UPROPERTY(BlueprintAssignable)
	FRequestSucceedDelegate SignUpRequestSucceedDelegate;
	UPROPERTY(BlueprintAssignable)
	FRequestSucceedDelegate ConfirmSignUpRequestSucceedDelegate;
	UPROPERTY(BlueprintAssignable)
	FRequestSucceedDelegate SignInRequestSucceedDelegate;
	UPROPERTY(BlueprintAssignable)
	FRefreshTokensRequestSucceedDelegate RefreshTokensRequestSucceedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FRequestSucceedDelegate SignOutRequestSucceedDelegate;
	
	UPROPERTY()
	FDSSignUp LastDSSignUp;
	UPROPERTY()
	FString Username_SignUp;
	
	UPROPERTY()
	FDSSignIn LastDSSignIn;
	UPROPERTY()
	FString Username_SignIn;
	UPROPERTY()
	FString Email_SignIn; //OPTIONAL, we eventually store it into DSSubsystem

	//some callbacks for WBP_SignOverlay::Buttons
		//this one directly bound to "DYNAMIC_DELEGATE" so it need to be "UFUNCTION"
	UFUNCTION()
	void QuitGame();

	void SendRequest_SignUp(const FString& Username, const FString& Email, const FString& Password, const FString& ConfirmPassword);
	void OnResponse_SignUp(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);
	
	void SendRequest_ConfirmSignUp(const FString& ConfirmationCode);	
	void OnResponse_ConfirmSignUp(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);
	
	void SendRequest_SignIn(const FString& Username, const FString& Password); //Username because of "SECRET_HASH"
	void OnResponse_SignIn(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);

	//Ironically you still need Username (as my client app is with "secretkey" so need secret_hash so need "Username" in its way)
	virtual void SendRequest_RefreshTokens(const FString& Username, const FString& RefreshToken) override;
	void OnResponse_RefreshTokens(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);

//Sign out
	void SendRequest_SignOut(const FString& AccessToken); //Sign Out don't need "SECRET_HASH"
	void OnResponse_SignOut(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);

protected:
	
public:
	
};
