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
    UE_LOG(GameServerLog, Log, TEXT("FDSGameSession {"));
    UE_LOG(GameServerLog, Log, TEXT("  CreationTime: %s"), *CreationTime);
    UE_LOG(GameServerLog, Log, TEXT("  CurrentPlayerSessionCount: %d"), CurrentPlayerSessionCount);
    UE_LOG(GameServerLog, Log, TEXT("  FleetArn: %s"), *FleetArn);
    UE_LOG(GameServerLog, Log, TEXT("  FleetId: %s"), *FleetId);

	/*OPTION1:
    UE_LOG(GameServerLog, Log, TEXT("  GameProperties (%d):"), GameProperties.Num());
    for (int32 i = 0; i < GameProperties.Num(); ++i)
    {
    	const auto& GP = GameProperties[i];
    	UE_LOG(GameServerLog, Log, TEXT("    [%d] %s = %s"), i, *GP.Key, *GP.Value);
    }

	UE_LOG(GameServerLog, Log, TEXT("  GameProperties (%d):"), GameProperties.Num());*/

	//OPTION2:
	int32 Index = 0;
	for (const TPair<FString, FString>& KVP : GameProperties)
	{
		UE_LOG(GameServerLog, Log, TEXT("    [%d] %s = %s"), Index++, *KVP.Key, *KVP.Value);
	}


    UE_LOG(GameServerLog, Log, TEXT("  GameSessionId: %s"), *GameSessionId);
    UE_LOG(GameServerLog, Log, TEXT("  IpAddress: %s"), *IpAddress);
    UE_LOG(GameServerLog, Log, TEXT("  Location: %s"), *Location);
    UE_LOG(GameServerLog, Log, TEXT("  MaximumPlayerSessionCount: %d"), MaximumPlayerSessionCount);
    UE_LOG(GameServerLog, Log, TEXT("  Name: %s"), *Name);
    UE_LOG(GameServerLog, Log, TEXT("  PlayerSessionCreationPolicy: %s"), *PlayerSessionCreationPolicy);
    UE_LOG(GameServerLog, Log, TEXT("  Port: %d"), Port);
    UE_LOG(GameServerLog, Log, TEXT("  Status: %s"), *Status);
    UE_LOG(GameServerLog, Log, TEXT("  TerminationTime: %s"), *TerminationTime);
    UE_LOG(GameServerLog, Log, TEXT("}"));
}

void FDSPlayerSession::Dump()
{
	UE_LOG(GameServerLog, Log, TEXT("FDSPlayerSession {"));
	UE_LOG(GameServerLog, Log, TEXT("  PlayerSessionId: %s"), *PlayerSessionId);
	UE_LOG(GameServerLog, Log, TEXT("  PlayerId:        %s"), *PlayerId);
	UE_LOG(GameServerLog, Log, TEXT("  Status:          %s"), *Status);

	UE_LOG(GameServerLog, Log, TEXT("  GameSessionId:   %s"), *GameSessionId);
	UE_LOG(GameServerLog, Log, TEXT("  FleetId:         %s"), *FleetId);
	UE_LOG(GameServerLog, Log, TEXT("  FleetArn:        %s"), *FleetArn);

	UE_LOG(GameServerLog, Log, TEXT("  IpAddress:       %s"), *IpAddress);
	UE_LOG(GameServerLog, Log, TEXT("  DnsName:         %s"), *DnsName);
	UE_LOG(GameServerLog, Log, TEXT("  Port:            %d"), Port);

	UE_LOG(GameServerLog, Log, TEXT("  PlayerData:      %s"), *PlayerData);

	UE_LOG(GameServerLog, Log, TEXT("  CreationTime:    %s"), *CreationTime);
	UE_LOG(GameServerLog, Log, TEXT("  TerminationTime: %s"), *TerminationTime);
	UE_LOG(GameServerLog, Log, TEXT("}"));

}
