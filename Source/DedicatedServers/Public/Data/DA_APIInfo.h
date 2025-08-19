// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DA_APIInfo.generated.h"
//struct FGameplayTag;

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UDA_APIInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/* For APIGateway::API_GroupX (APIGateway::API_X)
	 * this is format of a specific EndPoint/InvokeURL: [API_GroupX's URL]/stage/resource
	 * we will enter APIURL, stage, ResourceMap separately from BP
	 * Why don't we just enter the whole [API_GroupX's URL]/stage/resource?
	 * well because for a API_GroupX, at specific stage_i, we can have many resources and so can have many Endpoints/InvokeURLs
	 * so each UDA_APIInfo, at least in this course, will represent for API_X at a specific stage_i (not cover all stages if there is more than one, perhaps in this course we only have one stage)
	 */

	//For labeling Data, not used by any code
	UPROPERTY(EditDefaultsOnly)
	FString APIName;
	
	UPROPERTY(EditDefaultsOnly)
	FString APIURL;
	
	UPROPERTY(EditDefaultsOnly)
	FString Stage;
	UPROPERTY(EditDefaultsOnly)

	//resource in form of its tag (to ID it) and resource in form of FString (to be part of the InvokeURL: [APIURL]/stage/resource wil be in this map:
	TMap<FGameplayTag, FString> ResourceMap;

	FString GetInvokeURLByResourceTag(const FGameplayTag& InResourceTag);
};
