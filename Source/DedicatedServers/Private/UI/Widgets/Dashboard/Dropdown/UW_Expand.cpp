// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Dashboard/Dropdown/UW_Expand.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/RequestManager/RequestManager_Portal.h"


void UUW_Expand::NativeConstruct()
{
	Super::NativeConstruct();

	check(RequestManager_Portal_Class);
	RequestManager_Portal = NewObject<URequestManager_Portal>(this, RequestManager_Portal_Class);
 
	Button_SignOut->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
	Button_SignOut->OnHovered.AddDynamic(this, &ThisClass::OnButtonHovered);
	Button_SignOut->OnUnhovered.AddDynamic(this, &ThisClass::OnButtonUnhovered);
}

//so that no need to go to each mode of button and do it!
void UUW_Expand::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	FButtonStyle ButtonStyle;
	FSlateBrush ButtonBrush;
	ButtonBrush.TintColor = FSlateColor(FLinearColor(0,0,0,0)); //FColor(0,0,0,0) will also do
	ButtonStyle.Normal = ButtonBrush;
	ButtonStyle.Hovered = ButtonBrush;
	ButtonStyle.Pressed = ButtonBrush;
	ButtonStyle.Disabled = ButtonBrush;
	
	Button_SignOut->SetStyle(ButtonStyle);
	OnButtonUnhovered();
}

void UUW_Expand::OnButtonHovered()
{
	TextBlock_ButtonText->SetColorAndOpacity(Color_Hover);
}

void UUW_Expand::OnButtonUnhovered()
{
	TextBlock_ButtonText->SetColorAndOpacity(Color_Unhover);
}


//TODO: 
void UUW_Expand::OnButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("OnClicked SignOut!!!"), false);
//UNIVERSAL rule: always disable button that send requests, avoiding spamming it:
	//Button_SignOut->SetIsEnabled(false);
	
//access DSSystem:::AccessToken && call Portal::SendRequest_SignOut( DSSystem:::AccessToken )
	if (GetLocalPlayerSubsystem_DS() && IsValid(RequestManager_Portal))
	{
		RequestManager_Portal->SendRequest_SignOut(GetLocalPlayerSubsystem_DS()->GetAccessToken());		
	}
}

//why we can't make the function static? well because UObject::GetWorld() is different when call on different HOSTING object lol
ULocalPlayerSubsystem_DS* UUW_Expand::GetLocalPlayerSubsystem_DS() const
{
//get FirstLocalPlayer
	if (GetWorld() == nullptr) return nullptr;
	
 	ULocalPlayer* FirstLocalPlayer = GetWorld()->GetFirstLocalPlayerFromController(); //or GEngine->GetFirstGamePlayer(GetWorld());
	if (IsValid(FirstLocalPlayer))
	{
		return FirstLocalPlayer->GetSubsystem<ULocalPlayerSubsystem_DS>();	
	}
	return nullptr;;
}