// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DS_GameMode.generated.h"

struct FProcessParameters;

/*DECLARE_LOG_CATEGORY_EXTERN(GameServerLog, Log, All);*/

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ADS_GameMode();
	
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
