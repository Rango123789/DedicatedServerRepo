// Fill out your copyright notice in the Description page of Project Settings.
#include "Game/DSGameState.h"
#include "InfoAndState/LobbyInfo.h"
#include "Net/UnrealNetwork.h"

ADSGameState::ADSGameState()
{
	bReplicates = true;
	
}

void ADSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSGameState, LobbyInfo);
}

void ADSGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ConstructLobbyInfo();
		OnLobbyInfoInit.Broadcast(LobbyInfo); //we may want to do it here as well so that you may want to bind to _server::delegate for any reason.
	}
}

void ADSGameState::ConstructLobbyInfo()
{
	//GLOBALLY: GS::LobbyInfo pointer will be replicated from GS_server -> GS_clients!
	//LOCALLY: LobbyInfo is a self-replicated actor, hence the actor of spawning this actor will be replicated to all clients as well!
	//we only spawn it from the server and then 
	if (UWorld* World = GetWorld(); IsValid(World) && HasAuthority()) 	//funny, I didn't know such a thing is allowed:
	{
		//if you do "World->SpawnActor" only - this is LOCALLY self-replicated, in clients will have it as well
		//if you do LobbyInfo = "World->SpawnActor" , GLOBALLY the hosting class having its pointer replicated as well (it is marked ReplicatedUsing here as swell :) )
		LobbyInfo = World->SpawnActor<ALobbyInfo>(ALobbyInfo::StaticClass()); //this override will set location+ to "ZERO", yeah!

		if (IsValid(LobbyInfo))
		{
			LobbyInfo->SetOwner(this); //this also a self-replicated action (this is LOCALLY self-replicated, GLOBALLY the pointer isn't change hence no GLOBAL change)
		}
	}
}

/*
- the construction of the LobbyInfo that assign LobbyInfo = .... will NATURALLY trigger this OnRep_Pointer GLOBALLY (as I said above, i change from nullptr -> valid value)
- note that locally ALobbyInfo::OnRep_PlayerInfoArray() isn't actually trigger yet (not until we deliberately call ::AddPlayer++ on it), it is the spawn action of the AActor-child that is replicated
+the only reason that we even create this OnRep_Pointer is that when an external class like WBP_LobbyOverlay and whatnot trying to access AGameState too early and at the time it isn't constructing the LobbyInfo yet, so this give them a second chance.
+However I think we may want to try to do it in the constructor of this hosting class?
, well it may introduce any risk that I didn't know lol
, so anyway :)
*/
void ADSGameState::OnRep_LobbyInfo()
{
	OnLobbyInfoInit.Broadcast(LobbyInfo);
}
