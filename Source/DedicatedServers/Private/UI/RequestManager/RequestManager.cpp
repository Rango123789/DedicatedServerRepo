// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RequestManager/RequestManager.h"

#include "JsonObjectConverter.h"
#include "DedicatedServers/DedicatedServers.h"
#include "UI/RequestManager/HTTPRequestTypes.h"
#include "GameFramework/PlayerState.h"
#include "Player/LocalPlayerSubsystem_DS.h"

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

bool URequestManager::JsonStringToJsonObject(const FString& JsonString, TSharedPtr<FJsonObject>& OutJsonObject)
{
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	return FJsonSerializer::Deserialize( JsonReader, OutJsonObject);
}

bool URequestManager::JsonObjectToJsonString(const TSharedPtr<FJsonObject>& JsonObject, FString& OutJsonString)
{
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutJsonString); //JsonString is linked after this
	return FJsonSerializer::Serialize(JsonObject.ToSharedRef() , JsonWriter); //	FJsonObjectConverter::XToY won't work
}

//AI suggest the body even better than that i want :D :D
bool URequestManager::MapToJsonString(const TMap<FString, FString>& ContentMap, FString& OutJsonString)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	for (auto& Pair : ContentMap)
	{
		JsonObject->SetStringField(Pair.Key, Pair.Value);
	}
	return JsonObjectToJsonString(JsonObject, OutJsonString);
}

FString URequestManager::GetUniqueIdFromPlayerState()
{
	/*my DEMO idea: we trigging all these requests from a client, hence luckily all of these work :D :D
			//you don't want this, because even if it is DS, GetFirstPlayerController will return either of the client's PC
		GetWorld()->GetFirstPlayerController()->GetPlayerState<APlayerState>()->GetUniqueID();
			//you want this, DS doesn't many any ULocalPlayer, 
		GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->GetPlayerState<APlayerState>()->GetUniqueID();
		
			//this is an alternative if somehow you can't even "GetWorld()" in this HOSTING UObject lol? no in fact you need to pass in a GetWorld as well lol:
		GEngine->GetFirstLocalPlayerController(GetWorld())->GetPlayerState<APlayerState>()->GetUniqueID();
			//this is another lengthy alternative (GEngine can also GetFirstLocalPlayer as well - GetFirstGamePlayer is a bit strange lol, but it means the same thing)
		GEngine->GetFirstGamePlayer(GetWorld())->GetPlayerController(GetWorld())->GetPlayerState<APlayerState>()->GetUniqueID();
	 */
	
	//stephen's idea:
	/*PS::GetUniqueId -- return FUniqueNetIdRepl --this only for checking some condition (F
	  PS::GetUniqueID -- return uint32          --this is eventually what you need
	  --we face UniqueNetIdRepl in Shooter course, it is "a Wrapper for opaque type FUniqueNetId"
	  --you can review it later if you want*/
	if (GEngine &&
		GEngine->GetFirstLocalPlayerController(GetWorld()) &&
		GEngine->GetFirstLocalPlayerController(GetWorld())->GetPlayerState<APlayerState>() &&
		GEngine->GetFirstLocalPlayerController(GetWorld())->GetPlayerState<APlayerState>()->GetUniqueId().IsValid() // or PS::UniqueId
		)
	{
		uint32 UniqueID = GEngine->GetFirstLocalPlayerController(GetWorld())->GetPlayerState<APlayerState>()->GetUniqueID();
		FString PlayerUniqueId = FString::FromInt(UniqueID);
		return "Player_" + PlayerUniqueId; //let's prefix it.
	}
	
	return FString();
}

ULocalPlayerSubsystem_DS* URequestManager::GetLocalPlayerSubsystem_DS() const
{
	if (GetWorld() == nullptr) return nullptr;
	
//get FirstLocalPlayer
	ULocalPlayer* FirstLocalPlayer = GetWorld()->GetFirstLocalPlayerFromController(); //or GEngine->GetFirstGamePlayer(GetWorld());
	if (IsValid(FirstLocalPlayer))
	{
		return FirstLocalPlayer->GetSubsystem<ULocalPlayerSubsystem_DS>();	
	}
	return nullptr;;
}
