// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Dashboard/UW_GamePage.h"

#include "Components/Button.h"
#include "UI/RequestManager/RequestManager_GameSessions.h"
#include "UI/Widgets/API/GameSessions/UW_JoinGame.h"


void UUW_GamePage::NativeConstruct()
{
	Super::NativeConstruct();

	//construct RequestManager_GameSessions:
		//check(RequestManager_GameSessions_Class);
	RequestManager_GameSessions = NewObject<URequestManager_GameSessions>(this, RequestManager_GameSessions_Class);

	WBP_JoinGame->Button_JoinGame->OnClicked.AddDynamic(this, &ThisClass::UUW_GamePage::OnJoinGameButtonClicked);

	RequestManager_GameSessions->StatusMessageDelegate.AddDynamic(this, &ThisClass::OnStatusMessageDelegateBroadcast); 
}

void UUW_GamePage::OnJoinGameButtonClicked()
{
//side steps: I decide to do it first this time:
	WBP_JoinGame->Button_JoinGame->SetIsEnabled(false);

//main steps:
		//RequestManager_GameSessions->StatusMessageDelegate.AddDynamic(this, &ThisClass::OnStatusMessageDelegateBroadcast); -- better move up, you don't want to worry about unbind it lol
	RequestManager_GameSessions->SendRequest_FindAndJoinGameSession(); //I add "SendRequest_" so that it is more consistent.
}

//OPTIONAL: you can bind directly WBP_JoinGame::helper instead (i.e create helper from there instead)
void UUW_GamePage::OnStatusMessageDelegateBroadcast(const FString& StatusMessage, bool bResetWidgetButtons)
{
	WBP_JoinGame->SetStatusMessage(StatusMessage);
	//I decide to do it first this time:
	if(bResetWidgetButtons)
	{
		WBP_JoinGame->Button_JoinGame->SetIsEnabled(true);
	}
}
