
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RequestManager.generated.h"

class UDA_APIInfo;
/**
 * 
 */
UCLASS(Blueprintable)
class DEDICATEDSERVERS_API URequestManager : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDA_APIInfo> DA_APIInfo_GameSessions;
	
	bool CheckErrors(TSharedPtr<FJsonObject> JsonObject);
    void DumpMetadata(TSharedPtr<FJsonObject> JsonObject);
};
