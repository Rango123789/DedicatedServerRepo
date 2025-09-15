// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DSGameModeBase.h"
#include "GameFramework/GameMode.h"
#include "DSGameMode.generated.h"

struct FProcessParameters;

/*DECLARE_LOG_CATEGORY_EXTERN(GameServerLog, Log, All);*/

/**
 * at the same time we know that, from the main module, AShooterGameMode :  ADSGameMode : public ADSGameModeBase : AGameMode
 * this is rather crazy lol.
 */
UCLASS()
class DEDICATEDSERVERS_API ADSGameMode : public ADSGameModeBase //now inherit from this instead
{
	GENERATED_BODY()
public:
	ADSGameMode();
	
protected:
	virtual void BeginPlay() override;

private:
	void InitGameLift();
	/*
	void InitGameLift_Practice();
	*/
	
private:
	TSharedPtr<FProcessParameters> ProcessParams;
	
};
