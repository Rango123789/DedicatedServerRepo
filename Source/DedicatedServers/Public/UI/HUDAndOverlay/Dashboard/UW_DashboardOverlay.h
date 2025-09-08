// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_DashboardOverlay.generated.h"

class UButton;
class UUW_LeaderboardPage;
class UUW_CareerPage;
class UUW_GamePage;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_DashboardOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	/**
	 * 
	 */
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UUW_GamePage> WBP_GamePage;
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UUW_CareerPage> WBP_CareerPage;
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UUW_LeaderboardPage> WBP_LeaderboardPage;
	
	//for debugging purposes:
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UButton> Button_Game;
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UButton> Button_Career;
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UButton> Button_Leaderboard;

	//callbacks for those debugging buttons:
	UFUNCTION()
	void ShowGamePage();
	UFUNCTION()
	void ShowCareerPage();
	UFUNCTION()
	void ShowLeaderboardPage();
protected:
	virtual void NativeConstruct() override;
public:
	
};
