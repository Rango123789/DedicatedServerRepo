// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Types/DSTypes.h"
#include "DSPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimerStateChangedDelegate, ETimerType, TimerType, float, RemainingTime);

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ADSPlayerController();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void ReceivedPlayer() override;

	
	//we broadcast this when "we just start the timer + update the timer per Interval of 1.s (Timer+loop mode)"
	FOnTimerStateChangedDelegate OnTimerStateChangedDelegate_Update;
	//we broadcast this when "CountdownTime [Max -> 0]" (we should help to clear TimerHandle so it stops the loop above! EITHER in the callback of this delegate or next to where you broadcast this delegate)
	FOnTimerStateChangedDelegate OnTimerStateChangedDelegate_Finish;

	//these two are just for fun, we don't use it in this course:
	UPROPERTY(BlueprintAssignable)
	FOnTimerStateChangedDelegate OnTimerStateChangedDelegate_Pause;
	UPROPERTY(BlueprintAssignable)
	FOnTimerStateChangedDelegate OnTimerStateChangedDelegate_Stop;
	
//helper Client_RPCs to broadcast those delegates:
	UFUNCTION(Client, Reliable)
	void Client_BroadcastOnTimerStateChangedDelegate_Update(ETimerType TimerType, float RemainingTime);
	UFUNCTION(Client, Reliable)
	void Client_BroadcastOnTimerStateChangedDelegate_Finish(ETimerType TimerType, float RemainingTime);
	
protected:

	//ClientTimeWhenRequest = assuming pass in [GetWorld()->GetDeltaSeconds] from a client machine
	UFUNCTION(Server, Reliable)
	void Server_RequestServerTime(float ClientTimeWhenRequest); //Server_Ping
	// ServerTime = assuming pass in [GetWorld()->GetDeltaSeconds] from the server machine	
	UFUNCTION(Client, Reliable)
	void Client_ReceiveServerTime(float ServerTime, float ClientTimeWhenRequest); //Client_Pong

	//some of them may not be needed in this course, let's see.
	float RTT = 0.f;
	float Delta_ServerMinusClient = 0.f; //not used in this course?
	float CountingTime = 0.f;
	float PingInterval = 5.f;
};
