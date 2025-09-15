
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RequestManager.generated.h"

class ULocalPlayerSubsystem_DS;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusMessageDelegate, const FString&, StatusMessage, bool,
                                             bResetWidgetButtons);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRequestSucceedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRefreshTokensRequestSucceedDelegate , const FString&, InUsername, const FAuthenticationResult& , InAuthenticationResult);


class UDA_APIInfo;
/**
 * 
 */
UCLASS(Blueprintable)
class DEDICATEDSERVERS_API URequestManager : public UObject
{
	GENERATED_BODY()
public:
	/*
	UPROPERTY(BlueprintAssignable)
	FStatusMessageDelegate StatusMessageDelegate;*/
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDA_APIInfo> DA_APIInfo;
	
	bool CheckErrors(TSharedPtr<FJsonObject> JsonObject);
    void DumpMetadata(TSharedPtr<FJsonObject> JsonObject);
    
    static bool JsonStringToJsonObject(const FString& JsonString, TSharedPtr<FJsonObject>& OutJsonObject);
    static bool JsonObjectToJsonString(const TSharedPtr<FJsonObject>& JsonObject, FString& OutJsonString);
    
    //the temporary JsonObject will be created inside the helper itself so that you don't have to create it ourselves! yeah! (but the consequence is that I don't have the JsonObject itself to use outside when the need arises lol)
    static bool MapToJsonString(const TMap<FString, FString>& ContentMap, FString& OutJsonString);
	
	//this is from PlayerState::GetUniqueId(), not exactly "PlayerId" input from PlayerSession that can be anything you want (arbitrary but better off unique), stephen name it "GetUqniuePlayerId", but anyway:
	//this is just a placeholder function, we will use "PlayerId"=UserName when a player login instead :D  
	FString GetUniqueIdFromPlayerState();
	
	ULocalPlayerSubsystem_DS* GetLocalPlayerSubsystem_DS() const;
};
