// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTPRequestTypes.h"
#include "RequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "RequestManager_Portal.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusMessageDelegate, const FString& , StatusMessage, bool, bResetWidgetButtons);
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API URequestManager_Portal : public URequestManager
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
	
	UPROPERTY()
	FDSSignUp LastDSSignUp;
	UPROPERTY()
	FString LastUsername_SignUp;
	UPROPERTY()
	FString LastUsername_SignIn;

	UPROPERTY()
	FDSSignIn LastDSSignIn;
	
	//some callbacks for WBP_SignOverlay::Buttons
		//this one directly bound to "DYNAMIC_DELEGATE" so it need to be "UFUNCTION"
	UFUNCTION()
	void QuitGame();

	void SendRequest_SignUp(const FString& Username, const FString& Email, const FString& Password, const FString& ConfirmPassword);
	void OnResponse_SignUp(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);
	
	void SendRequest_ConfirmSignUp(const FString& ConfirmationCode);	
	void OnResponse_ConfirmSignUp(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);
	
	void SendRequest_SignIn(const FString& Username, const FString& Password);
	void OnResponse_SignIn(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);

	//Ironically you still need Username (as my client app is with "secretkey" so need secret_hash so need "Username" in its way)
	void SendRequest_RefreshTokens(const FString& Username, const FString& RefreshToken);
	void OnResponse_RefreshTokens(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);
	
protected:
	
public:
	
};
