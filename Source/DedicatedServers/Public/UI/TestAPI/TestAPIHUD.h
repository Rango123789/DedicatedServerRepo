// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TestAPIHUD.generated.h"

class UTestAPIOverlay;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ATestAPIHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTestAPIOverlay> TestAPIOverlay_Class;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTestAPIOverlay> TestAPIOverlay;
	
public:
	
};
