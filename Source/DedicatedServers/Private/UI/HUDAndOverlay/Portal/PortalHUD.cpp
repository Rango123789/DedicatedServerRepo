// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDAndOverlay/Portal/PortalHUD.h"
#include "UI/HUDAndOverlay/Portal/UW_SigninOverlay.h"
#include "Blueprint/UserWidget.h"
#include "UI/HUDAndOverlay/Dashboard/UW_DashboardOverlay.h"

void APortalHUD::BeginPlay()
{
	Super::BeginPlay();

	//you can't pass in this "HUD", because Created Widget only accept OwningObject = "PC/UWorld/UGameInstance" or "ToBeParentWidget" (used in  case you dynamically create a child widget to attach to a WBP_X::ParentWidget) -- see TestAPIOverlay to enjoy the case!
	//hence here you pass in PC as a good default option (perhaps better than GetWorld and surely better than GetGameInstance that hardly can be automatically destroyed lol)
	if (IsValid(PlayerOwner) && UW_SigninOverlay_Class) //i don't think this is needed
	{
		UW_SigninOverlay = CreateWidget<UUW_SigninOverlay>(PlayerOwner, UW_SigninOverlay_Class);
		UW_SigninOverlay->AddToViewport();
	}

	/*set InputMode = GameAndUI in PortalHUD::BeginPlay()
	 so that we can click on the button easier lol (experience from TestAPIHUD and its Overlay lol) */
	FInputModeGameAndUI InputModeGameAndUI;
	PlayerOwner->SetInputMode(InputModeGameAndUI);
	PlayerOwner->SetShowMouseCursor(true);
}

void APortalHUD::PostSignIn()
{
	//remove the old overlay from viewport:
	if(IsValid(UW_SigninOverlay)) UW_SigninOverlay->RemoveFromParent();
	
	//add the new one:
	if (IsValid(PlayerOwner) && UW_DashboardOverlay_Class) //i don't think this is needed
	{
		UW_DashboardOverlay = CreateWidget<UUW_DashboardOverlay>(PlayerOwner, UW_DashboardOverlay_Class);
		UW_DashboardOverlay->AddToViewport();
	}
}

//back to WBP_SignInOverlay
void APortalHUD::PostSignOut()
{
	//I want you to swap the role of UW_SigninOverlay and UW_DashboardOverlay here using PostSignIn as reference:
		// remove the dashboard overlay from viewport:
	if (IsValid(UW_DashboardOverlay)) UW_DashboardOverlay->RemoveFromParent();
	
	// add the sign-in overlay:
	if (IsValid(PlayerOwner) && UW_SigninOverlay_Class)
	{
		UW_SigninOverlay = CreateWidget<UUW_SigninOverlay>(PlayerOwner, UW_SigninOverlay_Class);
		UW_SigninOverlay->AddToViewport();
	}
}


