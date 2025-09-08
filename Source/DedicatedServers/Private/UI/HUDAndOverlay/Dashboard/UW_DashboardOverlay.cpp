// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDAndOverlay/Dashboard/UW_DashboardOverlay.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Widgets/Dashboard/UW_CareerPage.h"
#include "UI/Widgets/Dashboard/UW_GamePage.h"
#include "UI/Widgets/Dashboard/UW_LeaderboardPage.h"

void UUW_DashboardOverlay::ShowGamePage()
{
	WidgetSwitcher->SetActiveWidget(WBP_GamePage);
}

void UUW_DashboardOverlay::ShowCareerPage()
{
	WidgetSwitcher->SetActiveWidget(WBP_CareerPage);
}

void UUW_DashboardOverlay::ShowLeaderboardPage()
{
	WidgetSwitcher->SetActiveWidget(WBP_LeaderboardPage);
}

void UUW_DashboardOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Game->OnClicked.AddDynamic(this, &ThisClass::ShowGamePage);
	Button_Career->OnClicked.AddDynamic(this, &ThisClass::ShowCareerPage);
	Button_Leaderboard->OnClicked.AddDynamic(this, &ThisClass::ShowLeaderboardPage);
	
}
