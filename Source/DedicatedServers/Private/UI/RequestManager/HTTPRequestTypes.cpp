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

	//OPTION1:
    UE_LOG(GameServerLog, Log, TEXT("  GameProperties (%d):"), GameProperties.Num());
    for (int32 i = 0; i < GameProperties.Num(); ++i)
    {
    	const auto& GP = GameProperties[i];
    	UE_LOG(GameServerLog, Log, TEXT("    [%d] %s = %s"), i, *GP.Key, *GP.Value);
    }

	/*OPTION2:
	UE_LOG(GameServerLog, Log, TEXT("  GameProperties (%d):"), GameProperties.Num());
	int32 Index = 0;
	for (const TPair<FString, FString>& KVP : GameProperties)
	{
		UE_LOG(GameServerLog, Log, TEXT("    [%d] %s = %s"), Index++, *KVP.Key, *KVP.Value);
	}*/

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

void FDSSignIn::Dump() const
{
    UE_LOG(LogTemp, Display, TEXT("FDSSignIn {"));
    UE_LOG(LogTemp, Display, TEXT("  Session: %s"), Session.IsEmpty() ? TEXT("<empty>") : *Session);
    UE_LOG(LogTemp, Display, TEXT("  ChallengeName: %s"), ChallengeName.IsEmpty() ? TEXT("<empty>") : *ChallengeName);

    // AvailableChallenges
    if (AvailableChallenges.Num() == 0)
    {
        UE_LOG(LogTemp, Display, TEXT("  AvailableChallenges: []"));
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("  AvailableChallenges ["));
        for (int32 Index = 0; Index < AvailableChallenges.Num(); ++Index)
        {
            const FString& Item = AvailableChallenges[Index];
            UE_LOG(LogTemp, Display, TEXT("    [%d]: %s"), Index, Item.IsEmpty() ? TEXT("<empty>") : *Item);
        }
        UE_LOG(LogTemp, Display, TEXT("  ]"));
    }

    // ChallengeParameters
    if (ChallengeParameters.Parameters.Num() == 0)
    {
        UE_LOG(LogTemp, Display, TEXT("  ChallengeParameters: {}"));
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("  ChallengeParameters {"));
        for (const TPair<FString, FString>& Kvp : ChallengeParameters.Parameters)
        {
            UE_LOG(LogTemp, Display, TEXT("    %s: %s"),
                Kvp.Key.IsEmpty() ? TEXT("<empty>") : *Kvp.Key,
                Kvp.Value.IsEmpty() ? TEXT("<empty>") : *Kvp.Value);
        }
        UE_LOG(LogTemp, Display, TEXT("  }"));
    }

    // AuthenticationResult
    UE_LOG(LogTemp, Display, TEXT("  AuthenticationResult {"));
    UE_LOG(LogTemp, Display, TEXT("    AccessToken: %s"),
        AuthenticationResult.AccessToken.IsEmpty() ? TEXT("<empty>") : *AuthenticationResult.AccessToken);
    UE_LOG(LogTemp, Display, TEXT("    ExpiresIn: %d"), AuthenticationResult.ExpiresIn);
    UE_LOG(LogTemp, Display, TEXT("    IdToken: %s"),
        AuthenticationResult.IdToken.IsEmpty() ? TEXT("<empty>") : *AuthenticationResult.IdToken);
    UE_LOG(LogTemp, Display, TEXT("    RefreshToken: %s"),
        AuthenticationResult.RefreshToken.IsEmpty() ? TEXT("<empty>") : *AuthenticationResult.RefreshToken);
    UE_LOG(LogTemp, Display, TEXT("    TokenType: %s"),
        AuthenticationResult.TokenType.IsEmpty() ? TEXT("<empty>") : *AuthenticationResult.TokenType);
    UE_LOG(LogTemp, Display, TEXT("  }"));

    UE_LOG(LogTemp, Display, TEXT("}"));
}

void FDSSignUp::Dump() const
{
    UE_LOG(LogTemp, Display, TEXT("FDSSignUp {"));
    UE_LOG(LogTemp, Display, TEXT("  Session: %s"), Session.IsEmpty() ? TEXT("<empty>") : *Session);
    UE_LOG(LogTemp, Display, TEXT("  UserConfirmed: %s"), UserConfirmed ? TEXT("true") : TEXT("false"));
    UE_LOG(LogTemp, Display, TEXT("  UserSub: %s"), UserSub.IsEmpty() ? TEXT("<empty>") : *UserSub);

    UE_LOG(LogTemp, Display, TEXT("  CodeDeliveryDetails {"));
    UE_LOG(LogTemp, Display, TEXT("    AttributeName: %s"),
        CodeDeliveryDetails.AttributeName.IsEmpty() ? TEXT("<empty>") : *CodeDeliveryDetails.AttributeName);
    UE_LOG(LogTemp, Display, TEXT("    DeliveryMedium: %s"),
        CodeDeliveryDetails.DeliveryMedium.IsEmpty() ? TEXT("<empty>") : *CodeDeliveryDetails.DeliveryMedium);
    UE_LOG(LogTemp, Display, TEXT("    Destination: %s"),
        CodeDeliveryDetails.Destination.IsEmpty() ? TEXT("<empty>") : *CodeDeliveryDetails.Destination);
    UE_LOG(LogTemp, Display, TEXT("  }"));

    UE_LOG(LogTemp, Display, TEXT("}"));
}

void FAuthenticationResult::Dump() const
{
    UE_LOG(LogTemp, Log, TEXT("AuthenticationResult:"));
    UE_LOG(LogTemp, Log, TEXT("  AccessToken: %s"), *AccessToken);
    UE_LOG(LogTemp, Log, TEXT("  IdToken: %s"), *IdToken);
    UE_LOG(LogTemp, Log, TEXT("  RefreshToken: %s"), *RefreshToken);
}