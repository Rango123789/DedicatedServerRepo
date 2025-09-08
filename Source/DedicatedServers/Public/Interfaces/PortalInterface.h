// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/LocalPlayerSubsystem_DS.h"
#include "UObject/Interface.h"
#include "PortalInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPortalInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEDICATEDSERVERS_API IPortalInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SendRequest_RefreshTokens(const FString& Username, const FString& RefreshToken){};
};
