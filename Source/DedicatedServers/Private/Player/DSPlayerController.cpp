// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/DSPlayerController.h"

ADSPlayerController::ADSPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ADSPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

//do it here, so that you don't need "IsLocalController" check all the time :D
void ADSPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CountingTime+= DeltaTime;
	if (CountingTime >= PingInterval)
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
		CountingTime = 0.f;
	}
}

//so that you have RTT as soon as possible (avoiding "early artifact")
//[GPT] ReceivedPlayer() Called once when the server acknowledges this PlayerController belongs to a client.
//I double check it! in Shooter course we also use this one, NOT "OnPosseses" (that happen after this ReceivedPlayer, that is only when a char is spawned and this PC possesses it - way after ReceivePlayer <=> receive PlayerController)
void ADSPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	//new! learn alongside with AActor::Role (ENetRole) - it is private so you must use "GetRolePropertyName()" - anyway we never use it directly anyway lol. No need to do this when currently a device run "standardlone" game:
	if (GetNetMode() == ENetMode::NM_Standalone) return;
	
	//UNIVERSAL rule, always put EITHER IsLocalController or HasAuthority (typically NOT both) in PC::Built-inMethods/DelegateCallbacks as GOOD PRACTICE, even if you know it 'MAY' only trigger on the wanted device only!
	if(IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());	
	}
}

//callbacks for these delegates aren't created or bound yet!
void ADSPlayerController::Client_BroadcastOnTimerStateChangedDelegate_Update_Implementation(ETimerType TimerType,
	float RemainingTime)
{
	//when it reaches client, you know RemainingTime should be even more serious, minus RTT/2 :D
	OnTimerStateChangedDelegate_Update.Broadcast(TimerType, RemainingTime - RTT/2.f);
}

void ADSPlayerController::Client_BroadcastOnTimerStateChangedDelegate_Finish_Implementation(ETimerType TimerType,
	float RemainingTime)
{
	OnTimerStateChangedDelegate_Finish.Broadcast(TimerType, RemainingTime - RTT/2.f);
}

//RTT stuff:
void ADSPlayerController::Server_RequestServerTime_Implementation(float ClientTimeWhenRequest)
{
	if (HasAuthority()) return;
	
	Client_ReceiveServerTime(GetWorld()->GetTimeSeconds(), ClientTimeWhenRequest);
}

	//this is where to save RTT and Delta_ServerMinusClient:
void ADSPlayerController::Client_ReceiveServerTime_Implementation(float ServerTime, float ClientTimeWhenRequest)
{
	//for fun this one never chance, since we return very early
	if (HasAuthority())
	{
		RTT = 0.f;
		Delta_ServerMinusClient = 0.f;
	}
	else
	{
		RTT = GetWorld()->GetTimeSeconds() - ClientTimeWhenRequest;
		
		/*Server always enter the map first, hence its time running before any possible client connect and join it. Hence Delta_ServerMinusClient = [GetTimeSeconds]_server -  [GetTimeSeconds]_client > 0
		//each device has their own GetWorld() instance and can be constructed at different time, hence [GetTimeSeconds] so different in different clients and in client and server when comparing on the same world they're in:
		//why +RTT/2? because 'ServerTime' is passed in at "RTT/2" ago, now when this reaches the client here, it should be +RTT/2 already:*/
		Delta_ServerMinusClient = (ServerTime + RTT/2) - GetWorld()->GetTimeSeconds();
	}
}

