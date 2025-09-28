// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RequestManager.h"
#include "RequestManager_GameStats.generated.h"

class IHttpResponse;
class IHttpRequest;
struct FDSRecordMatchStats;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API URequestManager_GameStats : public URequestManager
{
	GENERATED_BODY()

public:
	void OnResponse_RecordGameStats(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bSuccessful);
	void SendRequest_RecordGameStats(const FDSRecordMatchStats& DSRecordGameStats);	
};








