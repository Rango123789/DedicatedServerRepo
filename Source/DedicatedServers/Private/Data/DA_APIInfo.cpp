// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/DA_APIInfo.h"



FString UDA_APIInfo::GetInvokeURLByResourceTag(const FGameplayTag& InResourceTag)
{
	//let it crashes if the map didn't have the specify tag. Because the [] operator do .FindChecked locally! yeah!
	return APIURL + "/" + Stage + "/" + ResourceMap[InResourceTag];
}
