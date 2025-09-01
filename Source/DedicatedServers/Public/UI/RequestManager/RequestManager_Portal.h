// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTPRequestTypes.h"
#include "RequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "RequestManager_Portal.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusMessageDelegate, const FString& , StatusMessage, bool, bResetJoinGameButton);
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
	
	//some callbacks for WBP_SignOverlay::Buttons
		//this one directly bound to "DYNAMIC_DELEGATE" so it need to be "UFUNCTION"
	UFUNCTION()
	void QuitGame(); 
		//indirectly called
	void SignIn(const FString& Username, const FString& Password); 
	void SignUp(const FString& UserName, const FString& Email, const FString& Password, const FString& ConfirmPassword);	
	void Confirm(const FString& VerificationCode);	
	
protected:
	
public:
	
};
