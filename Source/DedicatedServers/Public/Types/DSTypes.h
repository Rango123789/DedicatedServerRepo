// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DSTypes.generated.h"

/*this is meant to be used in place of GM::MatchState ??? NOT exactly! but Well yes lol
, Shocking news: [PreMatch - Match - PostMatch] states ARE IN FACT within "GM::InProgress" state (hence the comment)
, as long as we don't set "GM::bDelay=true" it go from [GM::EnteringMap->WaitingToStart->InProgress] directly with WaitingToStart=0 time
, meaning PreMatch status is NOT exactly GM::WaitingToStart AT ALL
+PreMatch doesn't exacr match WaitingtoStatt
*/
UENUM(BlueprintType)
enum class EMatchStatus : uint8 //you can call them EGameStatus of you want
{
	WaitingForPlayers, //we don't use "none" for this one, or replace "none" for "waitingForPlayers" if you want
	PreMatch, //but this isn't exactly GM::WaitingToStart 
	Match,
	PostMatch,
	SeamlessTravelBackToLobby //this may match GM::LeavingMap, but let's consider it still in GM::InProgress and just before GM::LeavingMap
};

/** DO NOT confuse:
- TimerState[/TimerStatus] here is to serve "TimerHandle" at its first purpose: whether it is started yet? it is running is being paused for reason?
, hence its value set isn't exactly matching MatchState
- TimerType is is to server MatchState::{WatingToStart, Pre-Match, Match, Post-Match} in Game map + in Lobby map
, hence its value set is matching MatchState + extra
 */

//you in fact don't need to add UMETA if you want the name to be shown in BP is exactly the same the C++ name here lol:
UENUM(BlueprintType)
enum class ETimerType : uint8
{
	None,           //I always put default value on top.
	LobbyCountdown, //I consider it "PostLobby", where Lobby and PreLobby didn't exist for timing, we don't have the need :D :D
	PreMatch,
	Match,
	PostMatch
};

//it looks like we don't have "Continue", if we "Paused" and want to continue, we set it back to "Started" :)
UENUM(BlueprintType)
enum class ETimerState : uint8
{
	NotStarted UMETA(DisplayName = "Not Started"),  //I always put default value on top.
	Started UMETA(DisplayName = "Started"),
	Paused UMETA(DisplayName = "Paused"),
	Finished UMETA(DisplayName = "Finished")     //or "Stopped"
};

//stephen name it FCountdownTimerHandle, but I name it "FCountdownTimerWrapper", it will contain "2 timer handles" + "relevant things"
USTRUCT(BlueprintType)
struct DEDICATEDSERVERS_API FCountdownTimerWrapper
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	ETimerType TimerType = ETimerType::None; //set this from C++, or must add 'EditAnywhere' for it
	
	UPROPERTY(BlueprintReadWrite)
	ETimerState TimerState = ETimerState::NotStarted;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CountdownTime = 5.f; //just change this
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float UpdateInterval = 1.f; //typically don't need to change this

	FTimerHandle TimerHandle_Finish{};
	FTimerHandle TimerHandle_Update{};

	FTimerDelegate TimerDelegate_Finish{};
	FTimerDelegate TimerDelegate_Update{};
};

//follow Unreal style and define  == operator outside the main struct (for class? well it is up to you), "inline" is optional, just good for "small function", but the compiler is the one ultimately will decide whether it is inline or not anyway lol:
inline bool operator==(const FCountdownTimerWrapper& lhs, const FCountdownTimerWrapper& rhs)
{
	return lhs.TimerType == rhs.TimerType;
}