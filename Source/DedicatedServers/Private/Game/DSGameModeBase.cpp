// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DSGameModeBase.h"

#include "aws/gamelift/server/model/Player.h"
#include "Player/DSPlayerController.h"
#include "Slate/SGameLayerManager.h"
#include "Types/DSTypes.h"


/*Purpose:
+Assumes you have:
TimerWrapper::CountdownTime [Max->0]
TimerWrapper::TimerType
TimerWrapper::UpdateInterval = 1.f

+Start to countdown and calling:
PCs::Client_BroadcastOnTimerChangedDelegate_Update - call per second until stop [CountdownTime,  ... , 0]
PCs::Client_BroadcastOnTimerChangedDelegate_Finish - call when RemainingTime = 0 (in fact we use SetTimer technique to achieve this one!)

*/
void ADSGameModeBase::StartCountdownTimer(FCountdownTimerWrapper& InTimerWrapper)
{
//Stage1: start the timer, callback of it will be "StopTimer" to clean things up
	
	//you must capture by reference to make sure it works:
	/* review "delegate.BindWeakLambda([](){})"
	= only be useful when you do [&]
	, and all reference counts on all references used inside {} won't be increased
	, and this lambda won't stop back all used references being destroyed!

	= if you only need to use [=] or nothing at all []
	, then BindLambda or BindWeakLambda won't make any difference :D :D*/
	InTimerWrapper.TimerDelegate_Finish.BindWeakLambda(this, [&]()
	{
		//this will help clean thing up as well as do final PCs::Client_Broadcast(0.f)
		StopCountdownTimer(InTimerWrapper);

		//not a DIRECT bound callback, it is a virtual function to override to expand actions in child GM if needed
		OnCountDownTimerFinished(InTimerWrapper.TimerType); 
	});
	
	//at this point body function "may" look for TimerDelegate::callbacks already, hence you must/should bind callbacks to TimerDelegate before you call this function
	//this will start the timer [CountdownTime -> 0]
	GetWorldTimerManager().SetTimer(
		InTimerWrapper.TimerHandle_Finish,
		InTimerWrapper.TimerDelegate_Finish,
		InTimerWrapper.CountdownTime, //Delay
		false
	);

//stage2: during [CountdownTime -> 0] update it per Interval!
	InTimerWrapper.TimerDelegate_Update.BindWeakLambda(this, [&]()
	{
		UpdateCountdownTimer(InTimerWrapper);
		OnCountDownTimerUpdated(InTimerWrapper.TimerType); 
	});
	
	GetWorldTimerManager().SetTimer(
		InTimerWrapper.TimerHandle_Update,
		InTimerWrapper.TimerDelegate_Update,
		InTimerWrapper.UpdateInterval, //Delay
		true
		//0.f //OPTION1: InFirstDelay=0 so it start right away - well didn't work well for the first timer (may be a bit too early for TimerManager itself or relevant things like PC to be fully setup)
	);
	
	/*OPTION2: manually call it for the first time (in case InFirstDelay=0 doesn't take effect - it doesn't happen sometime lol)
	**UPDATE: in fact stephen has the same problem even after he actually call the callback for the first time
	hence it could be because the PC at the time isn't spawned or given to client yet
	= the we must:
	- use trick1 for wrapper1 (+1 when we set countdown time for GM::Wrapper_PreMatch)
	- use trick2 for wrapper2+3 (call the callback upfront)
	*/
	UpdateCountdownTimer(InTimerWrapper);
	OnCountDownTimerUpdated(InTimerWrapper.TimerType); 
}

void ADSGameModeBase::StopCountdownTimer(FCountdownTimerWrapper& InTimerWrapper)
{
//clean up our InTimerWrapper: if it is timer - then clear timer, if it is timerdelegate - the unbind
	/*
	+GetWorldTimerManager().ClearTimer(Handle) → cancel the actual timer + invalidates the handle.
    +Handle.Invalidate() → only resets your local handle; timer keeps running.

	//do not use this lol:
	InTimerWrapper.TimerHandle_Finish.Invalidate();
	InTimerWrapper.TimerHandle_Update.Invalidate();
	*/
	GetWorldTimerManager().ClearTimer(InTimerWrapper.TimerHandle_Finish);
	GetWorldTimerManager().ClearTimer(InTimerWrapper.TimerHandle_Update);

	//I don't bother to do if (.IsBound())
	InTimerWrapper.TimerDelegate_Finish.Unbind();
	InTimerWrapper.TimerDelegate_Update.Unbind();

//set TimerState to ::Finished (don't feel it is important now but let's see lol), but why we didn't even set this anywhere else yet lol? set InWrapper::TimerType=Started when calling this function?
	InTimerWrapper.TimerState = ETimerState::Finished;
	
//we still need to loop through all PCs to call Client_Broadcast (RemainningTime=0) right?
	// PlayerControllerIterator itself has operator bool(), hence you simply put it in for ( , [statement return bool] , )
	for (FConstPlayerControllerIterator PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
	{
		if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PCIterator->Get()))
		{
			DSPlayerController->Client_BroadcastOnTimerStateChangedDelegate_Finish(InTimerWrapper.TimerType, 0.f);
		}
	}
}

void ADSGameModeBase::UpdateCountdownTimer(FCountdownTimerWrapper& InTimerWrapper)
{
	for (FConstPlayerControllerIterator PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
	{
		if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PCIterator->Get()))
		{
			//this time need to calculate RemainingTime lol:
			float RemainingTime1 = InTimerWrapper.CountdownTime - GetWorldTimerManager().GetTimerElapsed(InTimerWrapper.TimerHandle_Finish); //stephen
			float RemainingTime2 = GetWorldTimerManager().GetTimerRemaining(InTimerWrapper.TimerHandle_Finish); //me
			
			DSPlayerController->Client_BroadcastOnTimerStateChangedDelegate_Update(InTimerWrapper.TimerType, RemainingTime2);
		}
	}
}

void ADSGameModeBase::OnCountDownTimerFinished(const ETimerType& InTimerType)
{
	//EMPTY for this base class
}

void ADSGameModeBase::OnCountDownTimerUpdated(const ETimerType& InTimerType)
{
	//EMPTY for this base class
}

