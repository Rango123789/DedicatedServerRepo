// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RequestManager/RequestManager.h"

#include "JsonObjectConverter.h"
#include "DedicatedServers/DedicatedServers.h"
#include "UI/RequestManager/HTTPRequestTypes.h"

bool URequestManager::CheckErrors(TSharedPtr<FJsonObject> JsonObject)
{
	if ( JsonObject->HasField(TEXT("errorType")) ||
		JsonObject->HasField(TEXT("errorMessage")))
	{
		//this is enough I think:
		FString errorType = JsonObject->HasField(TEXT("errorType")) ? JsonObject->GetStringField(TEXT("errorType")) : TEXT("Unknown Error");
		UE_LOG(GameServerLog, Error, TEXT("errorType: %s"), *errorType );
		
		//you don't want this. it has secret information
		FString errorMessage = JsonObject->HasField(TEXT("errorMessage")) ? JsonObject->GetStringField(TEXT("errorMessage")) : TEXT("Unknown Error Message");
		UE_LOG(GameServerLog, Error, TEXT("errorMessage: %s"), *errorMessage ); 
		return true; //return early if there is error!
	}

	if (JsonObject->HasField(TEXT("$fault")))
	{
		//this time we look for "name" field that actually contain the error in this case (just direclty look at the lambda test and you will see lol, no why):
		FString errorType = JsonObject->HasField(TEXT("name")) ? JsonObject->GetStringField(TEXT("name")) : TEXT("Unknown Error");
		UE_LOG(GameServerLog, Error, TEXT("errorType: %s"), *errorType );
		return true; //return early if there is error!
	}
	return false;
}

void URequestManager::DumpMetadata(TSharedPtr<FJsonObject> JsonObject)
{
	if(JsonObject->HasField(TEXT("$metadata")))
	{
		TSharedPtr<FJsonObject> MetadataJsonObject = JsonObject->GetObjectField(TEXT("$metadata"));
		FDSMetadata MetadataStruct;
		FJsonObjectConverter::JsonObjectToUStruct( MetadataJsonObject.ToSharedRef(), &MetadataStruct );
		MetadataStruct.Dump();
	}
}