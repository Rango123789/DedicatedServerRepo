// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDAndOverlay/Portal/UW_SigninOverlay.h"

#include "Components/Button.h"
#include "UI/RequestManager/RequestManager_Portal.h"
#include "UI/Widgets/API/GameSessions/UW_JoinGame.h"

void UUW_SigninOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(RequestManager_Portal_Class);
	RequestManager_Portal = NewObject<URequestManager_Portal>(this, RequestManager_Portal_Class);

	check(WBP_JoinGame)
	check(WBP_JoinGame->Button_JoinGame)
	WBP_JoinGame->Button_JoinGame->OnClicked.AddDynamic(this, &ThisClass::OnJoinGameButtonClicked);
}

void UUW_SigninOverlay::OnJoinGameButtonClicked()
{
//side steps: I decide to do it first this time:
	WBP_JoinGame->Button_JoinGame->SetIsEnabled(false);

//main steps:
	check(RequestManager_Portal)
	
	RequestManager_Portal->StatusMessageDelegate.AddDynamic(this, &ThisClass::OnStatusMessageDelegateBroadcast); 
	
	RequestManager_Portal->SendRequest_FindAndJoinGameSession(); //I add "SendRequest_" so that it is more consistent.
}


//must: STEPHEN not yet upto this(nor create the Manager::2nd Delegate yet)!
void UUW_SigninOverlay::OnJoinGameResponseReceived(const FDSGameSession& DSGameSession, bool bWasSuccessful)
{
//side steps:

//main steps:
	if (bWasSuccessful)
	{
		
	}
	else
	{
		
	}
}

void UUW_SigninOverlay::OnStatusMessageDelegateBroadcast(const FString& StatusMessage, bool bResetJoinGameButton)
{
	WBP_JoinGame->SetStatusMessage(StatusMessage);
	//I decide to do it first this time:
	if(bResetJoinGameButton)
	{
		WBP_JoinGame->Button_JoinGame->SetIsEnabled(true);
	}
}