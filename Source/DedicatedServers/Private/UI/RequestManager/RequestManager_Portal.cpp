// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RequestManager/RequestManager_Portal.h"
#include "Kismet/GameplayStatics.h"

//GetWorld() ---> HUD->WBP_Overlay->PortalManager  -- each decide will have each "GetWorld()" and so got a "[First]LocalPlayerController" (First because it could be that 2 players play on the same device in some game lol). the point is that make sure "GetWorld" in current UObject Hosting class work.
//note: GetFirstPlayerController will always return some PC even if it is DedicatedServer or ListenServer
//whereas Get[First]LOCALPlayerController only valid for clients or ListenServer (DS will get nullptr)
void URequestManager_Portal::QuitGame()
{
	//to quit game you need to get PC first:
	APlayerController* FirstLocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (FirstLocalPlayerController)
	{
		//bIngorePlatformRestrictions because some console platforms that don't allow to quit game - better set it to false, do not ignore it :D :D
		UKismetSystemLibrary::QuitGame(this, FirstLocalPlayerController, EQuitPreference::Quit, false);
	}
}

void URequestManager_Portal::SignIn(const FString& Username, const FString& Password)
{
	//TODO:
	FString string;
	for (auto Character : string)
	{
		
	}
}

void URequestManager_Portal::SignUp(const FString& UserName, const FString& Email, const FString& Password,
	const FString& ConfirmPassword)
{
	//TODO:
}

void URequestManager_Portal::Confirm(const FString& VerificationCode)
{
	//TODO:
}

