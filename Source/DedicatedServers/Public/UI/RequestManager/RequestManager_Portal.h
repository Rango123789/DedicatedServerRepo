// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "RequestManager_Portal.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusMessageDelegate, const FString& , StatusMessage, bool, bResetJoinGameButton);
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API URequestManager_Portal : public URequestManager
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintAssignable)
	FStatusMessageDelegate StatusMessageDelegate;
	
	void SendRequest_FindAndJoinGameSession();
	void OnResponse_FindOrCreateGameSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
protected:
	
public:
	
};
