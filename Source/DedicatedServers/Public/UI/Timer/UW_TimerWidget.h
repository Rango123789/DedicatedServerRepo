// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/DSTypes.h"
#include "UW_TimerWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_TimerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Timer;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnInitialized() override;

	//no need to add spaces, BP always now to space them up there lol.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTimerUpdate"))
	void K2_OnTimerUpdate(ETimerType Timer, float RemainingTime);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTimerFinish"))
	void K2_OnTimerFinish(ETimerType Timer, float RemainingTime);

	//stephen make these callback virtual, so that it can be override in C++ child or WBP_child if needed
	UFUNCTION()
	virtual void OnTimerUpdate(ETimerType InTimerType, float RemainingTime);
	UFUNCTION()
	virtual void OnTimerFinish(ETimerType InTimerType, float RemainingTime);

//for code mechanics
	bool bIsActive = false;
	float InternalRemainingTime = 0.f;

//to be set/custumized from BP for different WBP_TimerChild
	//we will have many different WBP_TimerChild_i, and we also broadcast PC::delegate1,2(TimerType) differently on different context, hence only the intended WBP_TimerChild_i should react to it!
	UPROPERTY(EditAnywhere)
	ETimerType TimerType;
	UPROPERTY(EditAnywhere)
	bool bHiddenWhenInActive = false;

	UPROPERTY(EditAnywhere)
	bool bShowMilliseconds = true;
	UPROPERTY(EditAnywhere)
	bool bAllowNegativeTime = true;
//side helpers:
	
	FString TimeSecondsToString(float InTimeSeconds);
public:
	
};
