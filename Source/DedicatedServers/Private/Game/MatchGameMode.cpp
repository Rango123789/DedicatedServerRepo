// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MatchGameMode.h"

AMatchGameMode::AMatchGameMode()
{
	//applied when call ServerTravel from this current level (having this gamemode instance) to other levels (regardless OtherGM's b__ value)
	bUseSeamlessTravel = true;
	MatchStatus = EMatchStatus::WaitingForPlayers; //no need I set in in .h file already

	//only 3 types of Timer are involved in the Game level useing this current game mode (Lobby's GM will care about the rest ETimerType::LobbyCountdown), the rest Wrapper_i::vars will be set in BP ()
	TimerWrapper_PreMatch.TimerType = ETimerType::PreMatch;
	TimerWrapper_Match.TimerType = ETimerType::Match;
	TimerWrapper_PostMatch.TimerType = ETimerType::PostMatch;
	
}

void AMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//this Hosting function trigger whenever a player join/connect to the current map (having this gamemode), so no point to StartPostMatch timer again when it is already started (i.e no longer in EMatchStatus::WaitingForPlayers)
	//theoretically we will start the counting down for PreMatch status when enough players (say at least 2+)
	//however we're current in Game level already, it is Lobby level's responsibility (that we didn't create yet)
	//this PostLogin will still trigger many times (trigger one per player join lol)
	//but now we enforce this condition (and change the MatchStatus inside it) so only the FIRST player join can be able to trigger this function and get passed the if check (DS don't have any Player for themself, so the FIRST/randdom-earlier client join will trigger and get passed the if check, next clients - which only just right after the first - will still trigger the hosting funciton but can't pass the if check any more :) )
	if (MatchStatus == EMatchStatus::WaitingForPlayers)
	{
		MatchStatus = EMatchStatus::PreMatch;
		StartCountdownTimer(TimerWrapper_PreMatch);
	}
}

/*Because this is child of ADSGameModeBase (Timer functionality), so it can override this:
  Remember no matter you "StartCountdownTimer" on what TimerWrapper_i doesn't matter, all of them ending calling "this same shared function" when timer reach! hence we need to check its MatchStatus to decide what to do! hell yeah!*/
void AMatchGameMode::OnCountDownTimerFinished(const ETimerType& InTimerType)
{
	Super::OnCountDownTimerFinished(InTimerType); //EMPTY

	/*I think EITHER of if(A && B) is redundant, if you analyze it you will see that 3 timers are NOT overlaping at all: [Timer1 -> Timer2 ->Timer3]:), so checking InTimer is redundant safety! SPECIFICALLY, I think the B is redundant.
	- Be aware that we're in GM that is "the first chain" of the PINLINE1, lol, so Wrapper::TimerType, RemainingTime is broadcast from here and then to PC and then to any WBP_TimerWidget put onto viewport (because it auto-access PC and bind its callbacks to it lol, it is crazy)
	- All WBP_TimerWidget_i will auto access PC::delegate1,2 and bind its callbacks to it (meaning when GM->PC::Delegates broadcast all WBP_TimerWidget_i::callback1,2 will trigger no matter it match the TimerType or not) - we can't help it :)
	- However it auto-returns early when the WBP_TimerWigdte_i::TimerType != GM::Wrapper_i::TimerType (this is where it broadcast TimerType), so it is locally handled already, so we no longer need to worry about it!
	- so checking InTimerType == ETimerType::PreMatch is clearly redudant*/
	if (MatchStatus == EMatchStatus::PreMatch && InTimerType == ETimerType::PreMatch)
	{
		MatchStatus = EMatchStatus::Match;
		StartCountdownTimer(TimerWrapper_Match);
	}
	
	if (MatchStatus == EMatchStatus::Match && InTimerType == ETimerType::Match)
	{
		MatchStatus = EMatchStatus::PostMatch;
		StartCountdownTimer(TimerWrapper_PostMatch);
	}
	
	if (MatchStatus == EMatchStatus::PostMatch && InTimerType == ETimerType::PostMatch)
	{
		MatchStatus = EMatchStatus::SeamlessTravelBackToLobby;

		//no TimerWrapper to call next, we go back to the Lobby level, setting State now kind of lame because the GM will de destroyed as we travel any way! but it can't hurt right lol.
			//TODO: OpenLevel(LobbyLevel):
	}
	
}



