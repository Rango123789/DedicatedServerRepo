// Fill out your copyright notice in the Description page of Project Settings.


#include "InfoAndState/LobbyInfo.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ALobbyInfo::ALobbyInfo()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bAlwaysRelevant = true;
	
}

void ALobbyInfo::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyInfo, LobbyPlayerInfoArray);
}

void ALobbyInfo::AddPlayerInfo(FLobbyPlayerInfo InLobbyPlayerInfo)
{
	LobbyPlayerInfoArray.AddPlayerInfo(InLobbyPlayerInfo);
}

void ALobbyInfo::RemovePlayerInfo(const FString& InUsername)
{
	LobbyPlayerInfoArray.RemovePlayerInfo(InUsername);
}


//NEXT LESSON: you may want NewArray for this OnRep
void ALobbyInfo::OnRep_LobbyPlayerInfoArray()
{
//find out it is added or removed case, and which one is added/removed exactly, and then broadcast _Added/Removed: (many ways to do it)
/***MY IDEA: shoter but hurt performance when NumOfPlayers increase***/
	//BroadcastLobbyPlayerChanges(LastLobbyPlayerInfoArray);

/**STEPHEN IDEA: better performance!**/	
	FLobbyPlayerInfoDelta PlayerInfoDelta = GetPlayerInfoDelta();
	for (auto& PlayerInfo : PlayerInfoDelta.AddedPlayerInfos)
	{
		//broadcast added delegate:
		OnPlayerInfoAdded.Broadcast(PlayerInfo);
	}
	for (auto& PlayerInfo : PlayerInfoDelta.RemovedPlayerInfos)
	{
		//broadcast removed delegate:
		OnPlayerInfoRemoved.Broadcast(PlayerInfo);
	}

//important:
	LastLobbyPlayerInfoArray = LobbyPlayerInfoArray;
}

//create and return Delta is a joke, you can directly broadcast the removed and added delegate right in this function :D :D
FLobbyPlayerInfoDelta ALobbyInfo::GetPlayerInfoDelta()
{
	FLobbyPlayerInfoDelta Delta;
//step1: extract ::Items into Map<Item.PlayerName, Item>.
	//you MUST/SHOULD use "pointer" (or reference=bad choice), so that it doesn't have to copy!
	TMap<FString, FLobbyPlayerInfo*> LastPlayerInfoMap;
	TMap<FString, FLobbyPlayerInfo*> CurrentPlayerInfoMap;

	//can do auto& if you use "T" for the OldValue
	for (FLobbyPlayerInfo& PlayerInfo : LastLobbyPlayerInfoArray.Items)
	{
		LastPlayerInfoMap.Add(PlayerInfo.Username, &PlayerInfo);
	}

	//but our array member is just "T", hence auto& is okay:
	for (FLobbyPlayerInfo& PlayerInfo : LobbyPlayerInfoArray.Items)
	{
		LastPlayerInfoMap.Add(PlayerInfo.Username, &PlayerInfo);
	}
	
//step2:
	//if it is in CURRENT array but not in last array so it is "ADDED" one
	for (auto& PlayerInfo : LobbyPlayerInfoArray.Items)
	{
		//TMap.Contains is "INSTANT" lookup! yeah!
		if (CurrentPlayerInfoMap.Contains(PlayerInfo.Username) == false)
		{
			Delta.AddedPlayerInfos.Add(PlayerInfo);
		}
	}

	//if it is in the LAST array but not in current array so it is "REMOVED" one
	for (auto& PlayerInfo : LastLobbyPlayerInfoArray.Items)
	{
		if (CurrentPlayerInfoMap.Contains(PlayerInfo.Username) == false)
		{
			//TMap.Contains is "INSTANT" lookup! yeah!
			Delta.RemovedPlayerInfos.Add(PlayerInfo);
		}
	}

	return Delta;
}

/*when you use TArray<CustomClass>.Contains(...), you need to define operator== for it! I talked about this once
, in the last lesson, if I use my way - I need to define one
, but if we use Stephen way - we don't even need to! wow!*/
void ALobbyInfo::BroadcastLobbyPlayerChanges()
{
	//if the current item is in the current array, but NOT in the LAST array, then this item is just ADDED.
		//strange, both "LobbyPlayerInfoArray.Items" or "LobbyPlayerInfoArray" work lol
	for (auto& PlayerInfo : LobbyPlayerInfoArray.Items) //apparently "auto& __ : LobbyPlayerInfoArray" also work
	{
		//WARNING: TArray.Contains must loop through all the array to see it, so it is not good lol
		if (LastLobbyPlayerInfoArray.Items.Contains(PlayerInfo) == false)
		{
			//broadcast added delegate:
			OnPlayerInfoAdded.Broadcast(PlayerInfo);
		}
	}

	//if the current item is in the LAST array, but not in the current array, then this item is just REMOVED
	for (auto& PlayerInfo : LastLobbyPlayerInfoArray.Items)
	{
		//WARNING: TArray.Contains must loop through all the array to see it, so it is not good lol
		if (LobbyPlayerInfoArray.Items.Contains(PlayerInfo) == false)
		{
			//broadcast removed delegate:
			OnPlayerInfoRemoved.Broadcast(PlayerInfo);
		}
	}
}