// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DSGameModeBase.h"

#include "aws/gamelift/server/GameLiftServerAPI.h"
#include "aws/gamelift/server/model/Player.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DSPlayerController.h"
#include "Slate/SGameLayerManager.h"
#include "Types/DSTypes.h"

//warning: if you're too inherit more GM from this parent, you may run into trouble, but anyway I will keep it here as long as I didn't have any trouble yet lol 
void ADSGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

#if WITH_GAMELIFT
	if(ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Exiting) )
	{
		//when remove a player session we only need to check its outcome at the most: (no need before it)
		Aws::GameLift::GenericOutcome RemovePlayerSession = Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_ANSI(*DSPlayerController->PlayerSessionId));

		//well stephen didn't do this at all, and I don't have the need too lol. what if we fail to remove a player session? will it go "INACTIVE" if no player in that player session? I hope so lol.
		if (RemovePlayerSession.IsSuccess() == false)
		{
			//do nothing so far.
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Failed to remove player session.");
		};
	}
#endif
}

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
		//I also try this to make sure that the GM isn't destroyed yet: (STE didn't have this)
		if (IsValid(this) == false) return;
		
		//not a DIRECT bound callback, it is a virtual function to override to expand actions in child GM if needed
			//warning: if there is a code that travel to OtherLevel, this GM is subject to be destroyed, which is dangerous, because this weaklambda trying to reference to this::InTimerWrapper
		OnCountDownTimerFinished(InTimerWrapper.TimerType);

	//TRY1: to I move it down here, it will be fine if "OnCountDownTimerFinished" didn't trigger the "ServerTravel" code for sure, otherwise it MAY NOT be fine unless "StopCountdownTimer" is "skipped" due to the GM being destroyed? no guarantee lol
		//this will help clean thing up as well as do final PCs::Client_Broadcast(0.f)
			//warning: this contains the code that unbind the delegate timer's callback that is currently being executed (still code below need to be exected), so I suggest you move it to be at the end of this HOSTING lambda, as well as we .Unbind code locally must be final as well!
		    //it works in PIE but crash in packaged build.
		//StopCountdownTimer(InTimerWrapper);

	//TRY2: you MANUALLY call "StopCountdownTimer(InTimerWrapper)" outside of its own chain, when PreMatch reach, and right before you call StartCountdownTimer on Match, you help to call "StopCountdownTimer(InTimerWrapper)" right here (so just outside of its own chain! yeah!)
	//TRY3: in "StopCountdownTimer(InTimerWrapper)" itself, I move the DelegateTimer.Unbind to final! even before PC_i->Client_RPC is sent! yeah!
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
	**UPDATE2: now it is a MUST, as we need to know which one is the first one!
	*/
		//UpdateCountdownTimer(InTimerWrapper);
		//OnCountDownTimerUpdated(InTimerWrapper.TimerType); 
	OnCountDownTimerStarted(InTimerWrapper.TimerType);
	//we now have Client_BroadcastOnTimerStateChangedDelegate_Start:
	for (FConstPlayerControllerIterator PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
	{
		if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PCIterator->Get()))
		{
			DSPlayerController->Client_BroadcastOnTimerStateChangedDelegate_Start(InTimerWrapper.TimerType, InTimerWrapper.CountdownTime);
		}
	}
}

void ADSGameModeBase::StopCountdownTimer(FCountdownTimerWrapper& InTimerWrapper)
{

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

//WARNING: you can[/should] NOT unbind a delegate's callback when it is currently being executed, so let's try to move .Unbind() to "FINAL"
	//clean up our InTimerWrapper: if it is timer - then clear timer, if it is timerdelegate - the unbind
	/*
	+GetWorldTimerManager().ClearTimer(Handle) → cancel the actual timer + invalidates the handle.
    +Handle.Invalidate() → only resets your local handle; timer keeps running.

	//do not use this lol:
	InTimerWrapper.TimerHandle_Finish.Invalidate();
	InTimerWrapper.TimerHandle_Update.Invalidate();
	*/
	GetWorldTimerManager().ClearTimer(InTimerWrapper.TimerHandle_Update); //so that no longer trigger in the nearest second.
	GetWorldTimerManager().ClearTimer(InTimerWrapper.TimerHandle_Finish); //should you do this?

	//I don't bother to do if (.IsBound())
	InTimerWrapper.TimerDelegate_Finish.Unbind();
	InTimerWrapper.TimerDelegate_Update.Unbind();
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

void ADSGameModeBase::OnCountDownTimerStarted(const ETimerType& InTimerType)
{
	//EMPTY for this base class
}

void ADSGameModeBase::TrySeamlessTravelToNewLevel(const TSoftObjectPtr<UWorld>& InDestinationLevel)
{
    //travel to Game map:
    /* TSoftObjectPtr<T> review
        - it has 2 methods chain that return the equivalent thing
            TSoftObjectPtr::GetAssetName(); //use this
            TSoftObjectPtr::ToSoftObjectPath().GetAssetName();  //lengthy */
    if (GIsEditor)
    {
        //OPTION1:
        UGameplayStatics::OpenLevelBySoftObjectPtr(this, InDestinationLevel, true); //bAbsolute here is not bUseSeamlessTravel at all lol
        //OPTION2: just hard code the InDestinationLevel name, not sure we need "Path/to/Level"? well you need it unless you put the map in "Content/Maps" folder lol! 
        //UGameplayStatics::OpenLevel(GetWorld(),  FName("/Game/ThirdPerson/Maps/ThirdPersonMap"), true); //the name is in "ThirdPersonMap"
        //OPTION3: get the name from TSoftObjectPtr<UWorld>:
        //UGameplayStatics::OpenLevel(GetWorld(), FName(InDestinationLevel.GetAssetName()), true); //work in PIE even if the name is without "/path/to"
    }
    else
    {
        GetWorld()->ServerTravel(InDestinationLevel.GetAssetName(), false); //bAbsolute here is not bUseSeamlessTravel at all
    }
}

void ADSGameModeBase::SetClientInputEnabled(bool bEnabled) const
{
	for (FConstPlayerControllerIterator PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
	{
		if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PCIterator->Get()))
		{
			DSPlayerController->Client_SetInputEnabled(bEnabled);
		}
	}
}
