// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DSGameState.generated.h"

class ALobbyInfo;

//we call it "Init" instead of "Replicated" because we only change the pointer from [nullptr->valid] value ONCE, and never change it again (unlike in Slash course):
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyInfoInit,  ALobbyInfo*, LobbyInfo);

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADSGameState : public AGameState
{
	GENERATED_BODY()
public:
	ADSGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ConstructLobbyInfo();

	UPROPERTY(BlueprintAssignable)
	FOnLobbyInfoInit OnLobbyInfoInit;

	//we 'MAY' want to create BP_DSGameState child to be selected in GameMode, in this course we don't need any extra function in BP.
	//however this is just optional, you can always select "ALobbyInfo" directly, and so use ALobbyInfo::StaticClass() for SpawnActor() 
		//UPROPERTY(EditAnywhere)
		//TSubclassOf<ALobbyInfo> LobbyInfo_Class;

	UPROPERTY(ReplicatedUsing=OnRep_LobbyInfo)
	TObjectPtr<ALobbyInfo> LobbyInfo;

	UFUNCTION()
	void OnRep_LobbyInfo();
protected:
	virtual void BeginPlay() override;
};
