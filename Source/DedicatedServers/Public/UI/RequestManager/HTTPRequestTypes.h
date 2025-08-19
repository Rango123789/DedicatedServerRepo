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
struct FDSListFleetsReponse
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











