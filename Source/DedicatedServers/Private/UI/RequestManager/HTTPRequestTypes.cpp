// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RequestManager/HTTPRequestTypes.h"

#include "DedicatedServers/DedicatedServers.h"

void FDSMetadata::Dump() const
{
	UE_LOG(GameServerLog, Log, TEXT("httpStatusCode: %d"), httpStatusCode);
	UE_LOG(GameServerLog, Log, TEXT("requestId: %s"),*requestId);
	UE_LOG(GameServerLog, Log, TEXT("attempts: %d"), attempts);
	UE_LOG(GameServerLog, Log, TEXT("totalRetryDelay: %d"), totalRetryDelay);
}

void FDSListFleetsReponse::Dump() const
{
	for (FString FleetId : FleetIds)
	{
		UE_LOG(GameServerLog, Log, TEXT("FleetId: %s"), *FleetId);	
	}

	if (!NextToken.IsEmpty())
	{
		UE_LOG(GameServerLog, Log, TEXT("nextToken: %s"),*NextToken);	
	}
}
