// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "RequestManager_GameSessions.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusMessageDelegate, const FString& , StatusMessage, bool, bResetJoinGameButton);
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API URequestManager_GameSessions : public URequestManager
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FStatusMessageDelegate StatusMessageDelegate;

	
	void SendRequest_FindAndJoinGameSession();
	void OnResponse_CreatePlayerSession(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful);
	
	void SendRequest_CreatePlayerSession(const FString& GameSessionId, const FString& PlayerId);
	void OnResponse_FindOrCreateGameSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleCreatePlayerSession(const FString& GameSessionId, const FString& Status); //sub factorized of the OnReponse_X above
	FTimerHandle TimerHandle;
};
