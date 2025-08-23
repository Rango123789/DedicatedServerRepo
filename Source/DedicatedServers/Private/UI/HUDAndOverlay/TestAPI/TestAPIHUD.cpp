// Fill out your copyright notice in the Description page of Project Settings.


//I thought we can do #include "TestAPIHUD.h" directly? well it in fact needs to start from the associate Public folder lol :D :D
#include "UI/HUDAndOverlay/TestAPI/TestAPIHUD.h"
#include "UI/HUDAndOverlay/TestAPI/TestAPIOverlay.h"
#include "Blueprint/UserWidget.h"

void ATestAPIHUD::BeginPlay()
{
	Super::BeginPlay();

	//I don't think we need "&& TestAPIOverlay_Class" let's it give warning when we forget to pick a class for it!
	//STE: PlayerControler could "not only" be "nullptr" but could also be "not-null but pending-kill"
	if (IsValid(PlayerOwner) && TestAPIOverlay_Class)
	{
		TestAPIOverlay = CreateWidget<UTestAPIOverlay>(PlayerOwner, TestAPIOverlay_Class);
		TestAPIOverlay->AddToViewport();
	}
	
}
