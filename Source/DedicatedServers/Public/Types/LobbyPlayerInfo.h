// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "LobbyPlayerInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct DEDICATEDSERVERS_API FLobbyPlayerInfo : public FFastArraySerializerItem
{
		//funny: GENERATED_USTRUCT_BODY is alias to GENERATED_BODY. hence use either of them is okay
	GENERATED_BODY()

	FLobbyPlayerInfo(){};
	FLobbyPlayerInfo(const FString& InUsername) : Username(InUsername) {};
	//Rider suggest this, and I think this is pretty cool!
		//FLobbyPlayerInfo(const FString& InPlayerId, const FString& InPlayerName, const FString& InPlayerAvatarUrl);
	UPROPERTY()
	FString Username;
};

inline bool operator == (const FLobbyPlayerInfo& lhs,  const FLobbyPlayerInfo&rhs){return lhs.Username == rhs.Username;}

USTRUCT(BlueprintType)
struct DEDICATEDSERVERS_API FLobbyPlayerInfoArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FLobbyPlayerInfo> Items;

	//unlike when I implement FCustomEffectContext that takes time to implement "NetSerialize()", here it is so simple lol:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
	   //this is the versatile helper from its parent:
	   return FastArrayDeltaSerialize<FLobbyPlayerInfo, FLobbyPlayerInfoArray>( Items, DeltaParms, *this );
	}

//the DOC don't have this, but you can additionally add more functions if you want:
	void AddPlayerInfo(FLobbyPlayerInfo InLobbyPlayerInfo);
	void RemovePlayerInfo(const FString& InUsername);
};

//we're trying to replicate a whole array (not just its sub member ::Items), hence it makes sense we work on "FLobbyPlayerInfoArray" here:
template<>
struct TStructOpsTypeTraits< FLobbyPlayerInfoArray > : public TStructOpsTypeTraitsBase2< FLobbyPlayerInfoArray >
{
       enum 
       {
			WithNetDeltaSerializer = true,
       };
};
