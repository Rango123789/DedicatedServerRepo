// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Timer/UW_TimerWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "Player/DSPlayerController.h"

//warning: from PC::Client_Broadcast... we already account for "RTT/2" So don't subtract it here any more lol:
//                    OnTimerStateChangedDelegate_Finish.Broadcast(TimerType, RemainingTime - RTT/2.f);
void UUW_TimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//For now:
	if (bIsActive)
	{
	//let's our widget to have a chance to show the "MAX" time, by doing before subtracting :)
		//OPTION1: showing RAW seconds lol
			//TextBlock_Timer->SetText(FText::FromString(FString::SanitizeFloat(InternalRemainingTime)));
		//OPTION2: using our custom helper
		TextBlock_Timer->SetText(FText::FromString(TimeSecondsToString(InternalRemainingTime)));
		
	//Now can subtract time:
		InternalRemainingTime -= InDeltaTime;
	}
}

/*NativeConstruct is other 'risky' alternative, called per 'AddToViewPort'
NativeOnInitialized called per 'CreateWidget' (i.e ONCE for a given existing object)
bind PC::OnTimerStateChangedDelegate_Finish, PC::OnTimerStateChangedDelegate_Finish to 2 callbacks of UUW_TimerWidget::callback1,2 . And then implement appropriately.
lest lesson [GM -broadcast-> PC::Delegates]        PINELINE1
this is [PC::Delegates -bound-> WBP_TimerWidget_X] PINELINE2*/
void UUW_TimerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(GetOwningPlayer()))
	{
		DSPlayerController->OnTimerStateChangedDelegate_Start.AddDynamic(this, &ThisClass::OnTimerStart);
		DSPlayerController->OnTimerStateChangedDelegate_Update.AddDynamic(this, &ThisClass::OnTimerUpdate);
		DSPlayerController->OnTimerStateChangedDelegate_Finish.AddDynamic(this, &ThisClass::OnTimerFinish);
		
	}

	if(bHiddenWhenInActive)
	{
		//OPTION1: GPT say this is the best option, it is NOT rendered at all!
		SetVisibility(ESlateVisibility::Hidden);
		//OPTION2: this is similar, because the TextBlock is the one to be rendered that is not rendered at all after this
			//TextBlock_Timer->SetVisibility(ESlateVisibility::Hidden);
		//OPTION3: this is worst, because it is still rendered with alpha=0. Crazy :D :D
			//TextBlock_Timer->SetOpacity(0.f);
		
		//let's make sure it start off empty as well (if you don't in WBP_Child lol)
		TextBlock_Timer->SetText(FText::FromString(""));	
	}
}

void UUW_TimerWidget::OnTimerStart(ETimerType InTimerType, float RemainingTime)
{
    if (TimerType != InTimerType) return; 
    
	bIsActive = true;
	InternalRemainingTime = RemainingTime; //stephen didn't have this.
		
	SetVisibility(ESlateVisibility::Visible); //set it to visible no matter what

	K2_OnTimerStart(InTimerType, RemainingTime);
}

void UUW_TimerWidget::OnTimerUpdate(ETimerType InTimerType, float RemainingTime)
{
    if (TimerType != InTimerType) return; 

	//the first one sure get "MAX" value? well no we did "RTT/2" right from [PC::delegate.broadcast] pineline, so you would never get the "MAX" lol, and this make sense, unless you allow CountdownTime+=RTT/2 for the server machine at first place lol :D :D - it looks like stephen would like to do it soon :D :D
	//UPDATE now you can remove these code since you have "OnTimerStart" lol!
	if (bIsActive == false) //more like bDoOnce...
	{
		bIsActive = true;
		InternalRemainingTime = RemainingTime; //stephen didn't have this.
		
		SetVisibility(ESlateVisibility::Visible); //set it to visible no matter what

		//Shocking news: you can in fact directly call here and then skip to edit the whole chain [GM->PC->UW_TimerWidget ]lol! = but anyway I just did it lol. very stupid of me lol:
			//K2_OnTimerStart(InTimerType, RemainingTime);
	}

	//I name it K2_HostingFunctionName (OnTimerUpdate) , not K2_SubFactorizedFunctionName (will be TimerUpdate - if you factorize)
	K2_OnTimerUpdate(InTimerType, RemainingTime);
}

//when it finishes we set bIsActive = false no matter what (Tick on this block stop running) 
void UUW_TimerWidget::OnTimerFinish(ETimerType InTimerType, float RemainingTime)
{
	if (TimerType != InTimerType) return;
	
	bIsActive = false;
	InternalRemainingTime = 0; //not sure we need so, but well I guess it should already close to "0.f" itself already at this point
	
	if (bHiddenWhenInActive)
	{
		SetVisibility(ESlateVisibility::Hidden); 
	}

	//Tick won't work any more after bIsActive=false, but perhaps it already reach very close to "0" already? yes but it is still not "clean" and it is an artifact lol :D :D. Note that you hardly see this when bHiddenWhenInActive=true that it hides the widget already lol. 
	TextBlock_Timer->SetText(FText::FromString(TimeSecondsToString(0.f)));

	K2_OnTimerFinish(InTimerType, RemainingTime);
}

//I did it manually in JS already, so it is not a big deal for me to do it here
//this helper is optional if you happy with the UKismetSystemLibrary::helper already, then you don't even need to create this custom helper
//but anyway this custom helper meant to make it even stronger:
FString UUW_TimerWidget::TimeSecondsToString(float InTimeSeconds)
{
	FString DisplayTime;

	//OPTION1: it doesn't make sense when our clock count back up when it is negative :D :D
	InTimeSeconds = bAllowNegativeTime ? InTimeSeconds : FMath::Abs(InTimeSeconds);
	//OPTION2: I prefer this
	if (InTimeSeconds < 0.f) InTimeSeconds = 0.f;
	
	//Convert a number of seconds into minutes:seconds.milliseconds format string (including leading zeroes) - to be exact min:second:centisecond (because the final 2 number is stripped out!)
	if (bShowMilliseconds) 
	{
		DisplayTime = UKismetStringLibrary::TimeSecondsToString(InTimeSeconds);
	}
	else //copy exactly the content of UKismetStringLibrary::TimeSecondsToString() but comment out the "Centiseconds" part!
	{
		// Determine whether to display this number as a negative
		const TCHAR* NegativeModifier = InTimeSeconds < 0.f? TEXT("-") : TEXT("");
		InTimeSeconds = FMath::Abs(InTimeSeconds);
		
		// Get whole minutes
		const int32 NumMinutes = FMath::FloorToInt(InTimeSeconds/60.f);
		// Get seconds not part of whole minutes
		const int32 NumSeconds = FMath::FloorToInt(InTimeSeconds-(NumMinutes*60.f));
		
		/* //Get fraction of non-whole seconds, convert to 100th of a second, then floor to get whole 100ths
		const int32 NumCentiseconds = FMath::FloorToInt((InTimeSeconds - FMath::FloorToFloat(InTimeSeconds)) * 100.f);
		   //Create string, including leading zeroes
		return FString::Printf(TEXT("%s%02d:%02d.%02d"), NegativeModifier, NumMinutes, NumSeconds, NumCentiseconds); */

		DisplayTime = FString::Printf(TEXT("%s%02d:%02d"), NegativeModifier, NumMinutes, NumSeconds);
	}

	return DisplayTime;
}



















