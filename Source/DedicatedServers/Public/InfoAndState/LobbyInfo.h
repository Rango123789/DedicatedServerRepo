// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Types/LobbyPlayerInfo.h"
#include "LobbyInfo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInfoChangedDelegate, const FLobbyPlayerInfo&, PlayerInfo);

USTRUCT()
struct FLobbyPlayerInfoDelta
{
	GENERATED_BODY()

	//don't know why here stephen create TArray<T> instead :)
	UPROPERTY()
	TArray<FLobbyPlayerInfo> AddedPlayerInfos{};
	UPROPERTY()
	TArray<FLobbyPlayerInfo> RemovedPlayerInfos{};
};

UCLASS()
class DEDICATEDSERVERS_API ALobbyInfo : public AInfo
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALobbyInfo();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerInfoChangedDelegate OnPlayerInfoAdded;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerInfoChangedDelegate OnPlayerInfoRemoved;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_LobbyPlayerInfoArray)
	FLobbyPlayerInfoArray LobbyPlayerInfoArray;
	
	//you can NOT add "OldValue" for OnRep_FastArraySerializer (for technical reason lol), hence if we need OldArray, we need to MANUALLY create one :D :D and Cache it at the bottom of OnRep_Array :D :D
	UFUNCTION()
	void OnRep_LobbyPlayerInfoArray();
	
	FLobbyPlayerInfoArray LastLobbyPlayerInfoArray;



	//when we say "ItemDelta" we mean "delta between array1 and array2", you change the param set so that it can be re-used better, but I don't see the scenario we need to re-use it :)
	FLobbyPlayerInfoDelta GetPlayerInfoDelta(); //STEPHEN IDEA:
	void BroadcastLobbyPlayerChanges(); //MY IDEA:

public:
	TArray<FLobbyPlayerInfo> GetPlayerInfos(){return LobbyPlayerInfoArray.Items;}
	FLobbyPlayerInfoArray GetPlayerInfoArray(){return LobbyPlayerInfoArray;}
	void AddPlayerInfo(FLobbyPlayerInfo InLobbyPlayerInfo);
	void RemovePlayerInfo(const FString& InUsername);
};

