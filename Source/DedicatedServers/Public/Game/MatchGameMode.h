// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DSGameModeBase.h"
#include "MatchGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API AMatchGameMode : public ADSGameModeBase
{
	GENERATED_BODY()
public:
	AMatchGameMode();
protected:
	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;
	
	//to be used in place of GM::MatchState (yes and no lol)
	UPROPERTY(BlueprintReadOnly)
	EMatchStatus MatchStatus = EMatchStatus::WaitingForPlayers;
	
	//3 timer wrappers: (you can either configure it all or partly or nothing HERE - BP taking all or the rest or nothing)
	UPROPERTY(EditAnywhere)
	FCountdownTimerWrapper TimerWrapper_PreMatch;
	UPROPERTY(EditAnywhere)
	FCountdownTimerWrapper TimerWrapper_Match;
	UPROPERTY(EditAnywhere)
	FCountdownTimerWrapper TimerWrapper_PostMatch;
	virtual void OnCountDownTimerFinished(const ETimerType& InTimerType) override;
	
	//for testing purpose
	UPROPERTY(EditAnywhere)
	bool bOverrideInitSeamlessTravelPlayer = false;

	//testing:
	UPROPERTY(EditAnywhere)
	bool bCallDisableInputAtStartInGameMode = false;
public:
	
};
