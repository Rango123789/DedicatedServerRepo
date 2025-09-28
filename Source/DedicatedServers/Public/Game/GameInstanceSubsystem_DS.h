// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameInstanceSubsystem_DS.generated.h"

struct FProcessParameters;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UGameInstanceSubsystem_DS : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	bool bAlreadyInit = false;
	TSharedPtr<FProcessParameters> CachedProcessParams;
};
