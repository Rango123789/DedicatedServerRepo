// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HTTPRequestTypes.generated.h"

/**trick: FAKE create to have stuff setup quickly lol
 * 
#include "CoreMinimal.h"
#include "UObject/Object.h"
UCLASS()
class DEDICATEDSERVERS_API UHTTPRequestTypes : public UObject
{
	GENERATED_BODY()
};
 */

/*if you define its value right here you don't need "extern [CurrentModule]_API"
if you define its value in the associate .cpp (or in fact any other files) then you do need those - exactly like we do in Shooter course when we add more vars into built-in MatchState namespace remember! so that when something trying to use, it won't get the TRASH value but the value assigned in the associate file! Also "const" not force you to assign value immediately in this case! yeah!

In Generic C++, you simply need to add "extern"

In UE5 C++, you would need to add "extern [CurrentModule_API]" altogether, For new UClass+ you need "[CurrentModule_API]" generally if you want "other EXTERNAL modules" to be able include and use it (otherwise you can't) - NEEDVerified

@@Question:why the struct below is NOT required any of it?
there could be many possible reasons:
(1) it is marked with USTRUCT()
(2) this file is with .generated.h
(3) it is NOT a standalone variable

then why DECLARE_GAMEPLAY_TAG and DEFINE_GAMEPLAY_TAG didn't need it?
= well the new var is added into an already existing tag object of Unreal using those macro
, so I guess it is the reason
*/

namespace HttpStatusMessage
{
	extern DEDICATEDSERVERS_API const FString SomethingWentWrong;
}

USTRUCT()
struct FDSMetadata
{
	GENERATED_BODY()
public:
	/* Many ways to know a type of a field
	 * OPTION1: run in debug mode, Property/Value { FJsonValue{TYPE} , Value }
	 * OPTION2: look at the Response of lambda from AWS (better of at ::Lambda, not at CloudWatch)
	 -number will NOT with "" --C++-->int32 (it is first "int" and so will be converted to int32)
	 -string will with "" --C++-->FString
	 -boolean could only be true/false (without "" if I recall correctly) 
	 -sub JsonObject will be with {} --C++-->FJsonObject.
	 You must follow the exact KEYs from AWS, its style you know lol. */
	UPROPERTY()
	int32 httpStatusCode{};
	UPROPERTY()
	FString requestId{};
	UPROPERTY()
	int32 attempts{};
	UPROPERTY()
	int32 totalRetryDelay{};

	void Dump() const;
};

USTRUCT()
struct FDSListFleetsResponse
{
	GENERATED_BODY()

	//VERY strange, sometimes it is AxxxBxxx and sometime it is axxxBxxx from AWS lol, so always double-check it
	UPROPERTY()
	TArray<FString> FleetIds{};

	//according to the DOC, it is also AxxxBxxx
	UPROPERTY()
	FString NextToken{};

	void Dump() const;
};


//I use AI to create them! yeah!
USTRUCT()
struct FDSGameProperty
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString Key{};
	
	UPROPERTY()
	FString Value{};
};


USTRUCT()
struct FDSGameSession
{
	GENERATED_BODY()
/*{
   "GameSession": { 
      "CreationTime": number,
      "CreatorId": "string",
      "CurrentPlayerSessionCount": number,
      "DnsName": "string",
      "FleetArn": "string",
      "FleetId": "string",
      "GameProperties": [ 
         { 
            "Key": "string",
            "Value": "string"
         }
      ],
      "GameSessionData": "string",
      "GameSessionId": "string",
      "IpAddress": "string",
      "Location": "string",
      "MatchmakerData": "string",
      "MaximumPlayerSessionCount": number,
      "Name": "string",
      "PlayerSessionCreationPolicy": "string",
      "Port": number,
      "Status": "string",
      "StatusReason": "string",
      "TerminationTime": number
   }
}*/

	//fill my members here:
	UPROPERTY()
	FString CreationTime{};

	UPROPERTY()
	int32 CurrentPlayerSessionCount{};

	UPROPERTY()
	FString FleetArn{};

	UPROPERTY()
	FString FleetId{};

	//OPTION1: my AI Assistant idea
	UPROPERTY()
	TArray<FDSGameProperty> GameProperties{};
	
	/*OPTION2:  stephen idea - it also works, but imperfect, I don't this is a good idea at all!
	UPROPERTY()
	TMap<FString, FString> GameProperties{}; */

	UPROPERTY()
	FString GameSessionId{};

	UPROPERTY()
	FString IpAddress{};

	UPROPERTY()
	FString Location{};

	UPROPERTY()
	int32 MaximumPlayerSessionCount{};

	UPROPERTY()
	FString Name{};

	UPROPERTY()
	FString PlayerSessionCreationPolicy{};

	UPROPERTY()
	int32 Port{};

	UPROPERTY()
	FString Status{};

	UPROPERTY()
	FString TerminationTime{};

	void Dump() const;
};

USTRUCT()
struct FDSPlayerSession
{
	GENERATED_BODY()

	//stephen choose double, but I choose int64. AWS timestamps = [ X milliseconds - epoch point]
	//UPDATE: it is in fact FString, we don't care how AWS count time internally.
	UPROPERTY()
	FString CreationTime{}; // epoch time

	UPROPERTY()
	FString DnsName{};

	UPROPERTY()
	FString FleetArn{};

	UPROPERTY()
	FString FleetId{};

	UPROPERTY()
	FString GameSessionId{};

	UPROPERTY()
	FString IpAddress{};

	UPROPERTY()
	FString PlayerData{};

	UPROPERTY()
	FString PlayerId{};

	UPROPERTY()
	FString PlayerSessionId{};

	UPROPERTY()
	int32 Port{};

	UPROPERTY()
	FString Status{};

	UPROPERTY()
	FString TerminationTime{}; 

	void Dump();
};

//sub struct for FDSSignIn
USTRUCT()
struct FAuthenticationResult
{
	GENERATED_BODY()

	UPROPERTY()
	FString AccessToken{};
	UPROPERTY()
	int32 ExpiresIn{}; //in seconds, and expiring time should be in limit of int32 so yeah
	UPROPERTY()
	FString IdToken{};
	UPROPERTY()
	TMap<FString,FString> NewDeviceMetadata{}; //or just create a FSubStruct TIRE3 lol
	UPROPERTY()
	FString RefreshToken{};
	UPROPERTY()
	FString TokenType{};

	void Dump() const;
};

USTRUCT()
struct FChallengeParameter
{
	GENERATED_BODY()
	
	UPROPERTY()
	TMap<FString, FString> Parameters{};
};

USTRUCT()
struct FDSSignIn
{
	GENERATED_BODY()
/*
{
   "AuthenticationResult": { 
      "AccessToken": "string",
      "ExpiresIn": number,
      "IdToken": "string",
      "NewDeviceMetadata": { 
         "DeviceGroupKey": "string",
         "DeviceKey": "string"
      },
      "RefreshToken": "string",
      "TokenType": "string"
   },
   "AvailableChallenges": [ "string" ],
   "ChallengeName": "string",
	
	//this is be considered { TMap< , > } do NOT confuse with  Key: [{} , {} ] 
   "ChallengeParameters": { 
      "string" : "string" 
   },
   "Session": "string"
}*/
	//Id, access, refress tokens are in here:
	UPROPERTY()
	FAuthenticationResult AuthenticationResult{};

	UPROPERTY()
	TArray<FString> AvailableChallenges{};
	UPROPERTY()
	FString ChallengeName{};

	/*you don't want to create a TMap<FString,FString> directly lol, from now on I follow the exact pattern in all tires, won't simplify it
	* @@note: this is "unclear" that we don't even have any name for the field
		, also the DOC didn't make it clear that we could have many pair of "string" : "string" in it
		, so for the best we assume it could be more than one pair
		, hence TMap<Fstring,FString> is perfect in any case.
	* */
	UPROPERTY()
	FChallengeParameter ChallengeParameters{}; 
	
	UPROPERTY()
	FString Session{};

	void Dump() const;
};

//substruct for FDSSignUp
USTRUCT()
struct FCodeDeliveryDetails
{
	GENERATED_BODY()

	UPROPERTY()
	FString AttributeName{};
	UPROPERTY()
	FString DeliveryMedium{};
	UPROPERTY()
	FString Destination{};
};

USTRUCT()
struct FDSSignUp
{
	GENERATED_BODY()
/*
{
   "CodeDeliveryDetails": { 
      "AttributeName": "string",
      "DeliveryMedium": "string",
      "Destination": "string"
   },
   "Session": "string",
   "UserConfirmed": boolean,
   "UserSub": "string"
}*/

	UPROPERTY()
	FCodeDeliveryDetails CodeDeliveryDetails{};

	UPROPERTY()
	FString Session{};
	
	UPROPERTY()
	bool UserConfirmed{};
	
	UPROPERTY()
	FString UserSub{};

	void Dump() const;
};



