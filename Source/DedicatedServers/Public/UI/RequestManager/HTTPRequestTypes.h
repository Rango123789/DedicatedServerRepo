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

	//fill my members here:
	UPROPERTY()
	FString CreationTime{};

	UPROPERTY()
	int32 CurrentPlayerSessionCount{};

	UPROPERTY()
	FString FleetArn{};

	UPROPERTY()
	FString FleetId{};

	/*OPTION1: my AI Assistant idea
	UPROPERTY()
	TArray<FDSGameProperty> GameProperties{};*/
	
	//OPTION2:  stephen idea - it also works, which is cool!
	UPROPERTY()
	TMap<FString, FString> GameProperties{}; 

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











