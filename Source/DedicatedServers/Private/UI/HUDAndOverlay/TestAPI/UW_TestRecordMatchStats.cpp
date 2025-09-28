// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDAndOverlay/TestAPI/UW_TestRecordMatchStats.h"

#include "Components/Button.h"
#include "UI/RequestManager/HTTPRequestTypes.h"
#include "UI/RequestManager/RequestManager_GameStats.h"
void UUW_TestRecordMatchStats::NativeConstruct()
{
	Super::NativeConstruct();

	RequestManager_GameStats = NewObject<URequestManager_GameStats>(this, RequestManager_GameStats_Class);

	if (Button_RecordMatchStats == nullptr || RequestManager_GameStats == nullptr) return;
	
	Button_RecordMatchStats->OnClicked.AddDynamic(this, &ThisClass::OnRecordMatchStatsButtonClicked);

}

void UUW_TestRecordMatchStats::OnRecordMatchStatsButtonClicked()
{

	if (RequestManager_GameStats->GetLocalPlayerSubsystem_DS() == nullptr) return;
	
	FDSRecordMatchStats DSRecordMatchStats;
	//DSRecordMatchStats.Username = RequestManager_GameStats->GetLocalPlayerSubsystem_DS()->Username;
	DSRecordMatchStats.Username = "TestUsername";

	DSRecordMatchStats.MatchStats.Hits = 5;
	DSRecordMatchStats.MatchStats.Misses = 10;
	
	RequestManager_GameStats->SendRequest_RecordGameStats(DSRecordMatchStats);
}
