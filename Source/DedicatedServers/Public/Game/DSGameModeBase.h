// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Types/DSTypes.h"
#include "DSGameModeBase.generated.h"

struct FCountdownTimerWrapper;
/**
 * "ADSGameModeBase" because it will be base class for all other game modes in this project, not because it inherit from AGameModeBase (it in fact inherit from AGameMode instead :D :D )
 */
UCLASS()
class DEDICATEDSERVERS_API ADSGameModeBase : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void Logout(AController* Exiting) override;
	
	//you must/should always use "T&" for delegate param or return, this is something I learnt from GAS course!
	void StartCountdownTimer(FCountdownTimerWrapper& InTimerWrapper);
	void StopCountdownTimer(FCountdownTimerWrapper& InTimerWrapper);
	void UpdateCountdownTimer(FCountdownTimerWrapper& InTimerWrapper);

	
	virtual void OnCountDownTimerStarted(const ETimerType& InTimerType); //MANUALLY call it outside of looping SetTimer
	virtual void OnCountDownTimerUpdated(const ETimerType& InTimerType); //call it inside callback of SetTimer
	
	virtual void OnCountDownTimerFinished(const ETimerType& InTimerType); 
	void TrySeamlessTravelToNewLevel(const TSoftObjectPtr<UWorld>& InDestinationLevel);

	void SetClientInputEnabled(bool bEnabled) const;
protected:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> LobbyMap;
public:
	
};
