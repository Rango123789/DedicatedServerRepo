// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTPRequestTypes.h"
#include "RequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "RequestManager_TestAPIOverlay.generated.h"

class IHttpResponse;
class IHttpRequest;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnListFleetsResponseReceivedDelegate , const FDSListFleetsResponse& , DSListFleetsResponse, bool, bWasSuccessful);
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API URequestManager_TestAPIOverlay : public URequestManager
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FOnListFleetsResponseReceivedDelegate	OnListFleetsResponseReceivedDelegate;

	void SendRequest_ListFleets();
	
	/*option1: must include the file defining the typedef, i.e HttpRequest.h - we choose this option in Shooter course' plugin remember?*/
	void OnResponse_ListFleets(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	/*Option2: then you don't want to include HttpRequest.h and HttpResponse here! or at least the file defining the typedef HttpRequestPtr and HttpResponsePtr --yes HttpRequest.h has both of them!
	- Question: why must we specify ESPMode::ThreadSafe? well because the default mode is ESPMode::NotThreadSafe lol
	ESPMode::NotThreadSafe → lighter, faster, but not safe across threads.
	ESPMode::ThreadSafe → uses atomics for ref counting, so safe to use across multiple threads, but has a tiny performance cost.
		void OnResponse_ListFleets_V2(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bWasSuccessful);
	*/
	
};
