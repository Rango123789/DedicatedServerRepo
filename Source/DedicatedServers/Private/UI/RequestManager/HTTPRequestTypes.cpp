// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RequestManager/HTTPRequestTypes.h"
#include "DedicatedServers/DedicatedServers.h"

namespace HttpStatusMessage
{
	const FString SomethingWentWrong = TEXT("Something went wrong");
}


void FDSMetadata::Dump() const
{
	UE_LOG(GameServerLog, Log, TEXT("Metadata:"));
	
	UE_LOG(GameServerLog, Log, TEXT("httpStatusCode: %d"), httpStatusCode);
	UE_LOG(GameServerLog, Log, TEXT("requestId: %s"),*requestId);
	UE_LOG(GameServerLog, Log, TEXT("attempts: %d"), attempts);
	UE_LOG(GameServerLog, Log, TEXT("totalRetryDelay: %d"), totalRetryDelay);
}

void FDSListFleetsResponse::Dump() const
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

void FDSGameSession::Dump() const
{
    UE_LOG(LogTemp, Log, TEXT("FDSGameSession {"));
    UE_LOG(LogTemp, Log, TEXT("  CreationTime: %s"), *CreationTime);
    UE_LOG(LogTemp, Log, TEXT("  CurrentPlayerSessionCount: %d"), CurrentPlayerSessionCount);
    UE_LOG(LogTemp, Log, TEXT("  FleetArn: %s"), *FleetArn);
    UE_LOG(LogTemp, Log, TEXT("  FleetId: %s"), *FleetId);

	/*OPTION1:
    UE_LOG(LogTemp, Log, TEXT("  GameProperties (%d):"), GameProperties.Num());
    for (int32 i = 0; i < GameProperties.Num(); ++i)
    {
    	const auto& GP = GameProperties[i];
    	UE_LOG(LogTemp, Log, TEXT("    [%d] %s = %s"), i, *GP.Key, *GP.Value);
    }

	UE_LOG(LogTemp, Log, TEXT("  GameProperties (%d):"), GameProperties.Num());*/

	//OPTION2:
	int32 Index = 0;
	for (const TPair<FString, FString>& KVP : GameProperties)
	{
		UE_LOG(LogTemp, Log, TEXT("    [%d] %s = %s"), Index++, *KVP.Key, *KVP.Value);
	}


    UE_LOG(LogTemp, Log, TEXT("  GameSessionId: %s"), *GameSessionId);
    UE_LOG(LogTemp, Log, TEXT("  IpAddress: %s"), *IpAddress);
    UE_LOG(LogTemp, Log, TEXT("  Location: %s"), *Location);
    UE_LOG(LogTemp, Log, TEXT("  MaximumPlayerSessionCount: %d"), MaximumPlayerSessionCount);
    UE_LOG(LogTemp, Log, TEXT("  Name: %s"), *Name);
    UE_LOG(LogTemp, Log, TEXT("  PlayerSessionCreationPolicy: %s"), *PlayerSessionCreationPolicy);
    UE_LOG(LogTemp, Log, TEXT("  Port: %d"), Port);
    UE_LOG(LogTemp, Log, TEXT("  Status: %s"), *Status);
    UE_LOG(LogTemp, Log, TEXT("  TerminationTime: %s"), *TerminationTime);
    UE_LOG(LogTemp, Log, TEXT("}"));
}
