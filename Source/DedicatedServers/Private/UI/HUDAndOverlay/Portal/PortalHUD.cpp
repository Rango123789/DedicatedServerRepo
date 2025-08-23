// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDAndOverlay/Portal/PortalHUD.h"
#include "UI/HUDAndOverlay/Portal/UW_SigninOverlay.h"
#include "Blueprint/UserWidget.h"

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
