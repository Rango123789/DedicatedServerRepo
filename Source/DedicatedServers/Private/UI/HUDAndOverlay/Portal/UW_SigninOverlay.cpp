// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDAndOverlay/Portal/UW_SigninOverlay.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Player/LocalPlayerSubsystem_DS.h"
#include "UI/RequestManager/RequestManager_Portal.h"
#include "UI/Widgets/API/GameSessions/UW_JoinGame.h"
#include "UI/Widgets/SignIn/UW_ConfirmSignUpPage.h"
#include "UI/Widgets/SignIn/UW_SignInPage.h"
#include "UI/Widgets/SignIn/UW_SignUpPage.h"
#include "UI/Widgets/SignIn/UW_SuccessConfirmSignUpPage.h"

void UUW_SigninOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(RequestManager_Portal_Class);
	RequestManager_Portal = NewObject<URequestManager_Portal>(this, RequestManager_Portal_Class);

/*To WBP_NewOverlay soon	
	//TODO: select BP_RequestManager_GameSessions class from BP for _Class
	check(RequestManager_GameSessions_Class);
	RequestManager_GameSessions = NewObject<URequestManager_GameSessions>(this, RequestManager_GameSessions_Class);

	WBP_JoinGame->Button_JoinGame->OnClicked.AddDynamic(this, &ThisClass::OnJoinGameButtonClicked);
*/

//bind callbacks (or lambdas) for debugging buttons:
	Button_Test_SignInPage->OnClicked.AddDynamic(this, &ThisClass::ShowSignInPage);
	Button_Test_SignUpPage->OnClicked.AddDynamic(this, &ThisClass::ShowSignUpPage);
	Button_Test_ConfirmSignUpPage->OnClicked.AddDynamic(this, &ThisClass::ShowConfirmSignUpPage);
	Button_Test_SuccessConfirmSignUpPage->OnClicked.AddDynamic(this, &ThisClass::ShowSuccessConfirmSignUpPage);

//bind callbacks on "back", "quit", "OK" , "Sign up: to SignUpPage":
	WBP_SignInPage->Button_SignUp->OnClicked.AddDynamic(this, &ThisClass::ShowSignUpPage); //forward to page2 (from page1)
	WBP_SignInPage->Button_Quit->OnClicked.AddDynamic(RequestManager_Portal, &URequestManager_Portal::QuitGame); //quit game, let PortalManager handle it

	WBP_SignUpPage->Button_Back->OnClicked.AddDynamic(this, &ThisClass::ShowSignInPage); //back to page1 (from page2)
	WBP_ConfirmSignUpPage->Button_Back->OnClicked.AddDynamic(this, &ThisClass::ShowSignUpPage); //back to page2 (from page3)
	WBP_SuccessConfirmSignUpPage->Button_OK->OnClicked.AddDynamic(this, &ThisClass::ShowSignInPage); //back to page1 (from page4) - not to page3
	
//bind callbacks on those buttons involving "sending requests to AWS::Cognito::UserPool":
	//HERE is the painful fact: Button::OnClicked is "DECLARE_DYNAMIC_DELEGATE" will only accept callback with "no params", but you need to pass in WBP_X::__::GetText() before you can call &URequestManager_Portal::SignIn( , ) with params
	//hence we need an intermediate callback from this HOSTING class before we can in turn call &URequestManager_Portal:Helpers( , ) with params!
	WBP_SignInPage->Button_SignIn->OnClicked.AddDynamic(this, &ThisClass::OnSignInButtonClicked); //forward to RequestManager_Portal::SignIn
	WBP_SignUpPage->Button_SignUp->OnClicked.AddDynamic(this, &ThisClass::OnSignUpButtonClicked); //forward to RequestManager_Portal::SignUp
	WBP_ConfirmSignUpPage->Button_Confirm->OnClicked.AddDynamic(this, &ThisClass::OnConfirmButtonClicked); //forward to RequestManager_Portal::SignUp

//we're not to just bind every time you click to send request and unbind it when we receive response lol - make code complex unnecessarily, so do it here:
	RequestManager_Portal->SignUpStatusMessageDelegate.AddDynamic( WBP_SignUpPage, &UUW_SignUpPage::UpdateStatusMessage);
	RequestManager_Portal->ConfirmSignUpStatusMessageDelegate.AddDynamic( WBP_ConfirmSignUpPage, &UUW_ConfirmSignUpPage::UpdateStatusMessage); 
	RequestManager_Portal->SignInStatusMessageDelegate.AddDynamic(WBP_SignInPage, &UUW_SignInPage::UpdateStatusMessage); //done!
	
	RequestManager_Portal->SignUpRequestSucceedDelegate.AddDynamic( this, &ThisClass::OnSignUpRequestSucceed);
	RequestManager_Portal->ConfirmSignUpRequestSucceedDelegate.AddDynamic(this, &ThisClass::OnConfirmSignUpRequestSucceed); 
	RequestManager_Portal->SignInRequestSucceedDelegate.AddDynamic(this, &ThisClass::OnSignInRequestSucceed); //TODO in next chapter
}

void UUW_SigninOverlay::OnSignInButtonClicked()
{
	//good time to disable the button: (perhaps enable it back when receive response)
	WBP_SignInPage->Button_SignIn->SetIsEnabled(false);
	
	//must take from the right page (SignInPage, not SignUpPage also have those fields lol)
	FText UserName = WBP_SignInPage->EditableTextBox_UserName->GetText();
	FText Password = WBP_SignInPage->EditableTextBox_Password->GetText();
	RequestManager_Portal->SendRequest_SignIn(UserName.ToString(), Password.ToString());
}

void UUW_SigninOverlay::OnSignUpButtonClicked()
{
	//good time to disable the button: (perhaps enable it back when receive response, but it can be also enabled back by entering qualified info)
	WBP_SignUpPage->Button_SignUp->SetIsEnabled(false);
	
	//the same logic here:
	FText UserName = WBP_SignUpPage->EditableTextBox_UserName->GetText();
	FText Email = WBP_SignUpPage->EditableTextBox_Email->GetText();
	FText Password = WBP_SignUpPage->EditableTextBox_Password->GetText();
	FText ConfirmPassword = WBP_SignUpPage->EditableTextBox_ConfirmPassword->GetText();
	RequestManager_Portal->SendRequest_SignUp(UserName.ToString(), Email.ToString(), Password.ToString(), ConfirmPassword.ToString());
}

void UUW_SigninOverlay::OnConfirmButtonClicked()
{
	//good time to disable the button: (perhaps enable it back when receive response, but it can be also enabled back by entering qualified info)
	WBP_ConfirmSignUpPage->Button_Confirm->SetIsEnabled(false);
	
	FText VerificationCode = WBP_ConfirmSignUpPage->EditableTextBox_VerificationCode->GetText();
	RequestManager_Portal->SendRequest_ConfirmSignUp(VerificationCode.ToString());
}

//you can consider to disable WBP_X::Button_key, when you call ShowWBP_X, except WBP_4 don't want that!
//you should also disable them by default (either in WBP_Overlay Editor or in NativeConstruct = I like in NativeConstruct)
void UUW_SigninOverlay::ShowSignInPage()
{
	//meaning if you disable SignIn button you need to do RegexPattern when OnTextChanged as well, so I don't implement it for now so I command this out
		//WBP_SignInPage->Button_SignIn->SetIsEnabled(false); 
	WidgetSwitcher->SetActiveWidget(WBP_SignInPage);
}
void UUW_SigninOverlay::ShowSignUpPage()
{
	WBP_SignUpPage->Button_SignUp->SetIsEnabled(false); 
	WidgetSwitcher->SetActiveWidget(WBP_SignUpPage);
}

void UUW_SigninOverlay::ShowConfirmSignUpPage()
{
	WBP_ConfirmSignUpPage->Button_Confirm->SetIsEnabled(false);
	WidgetSwitcher->SetActiveWidget(WBP_ConfirmSignUpPage);
}

//this one don't want it, there is no text box for "OnTextChanged" to enable it back if you disable it, you need button "OK" to jump back to WBP_1
void UUW_SigninOverlay::ShowSuccessConfirmSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(WBP_SuccessConfirmSignUpPage);
}

void UUW_SigninOverlay::OnSignUpRequestSucceed()
{
//clear all EditableTextBoxes in WBP_2:
	WBP_SignUpPage->ClearTextBoxes();
	
//jump to WBP_3:
	ShowConfirmSignUpPage();
	
//update the WBP_3::TextBlock_Destination as well
	FString Destination = RequestManager_Portal->LastDSSignUp.CodeDeliveryDetails.Destination;
	WBP_ConfirmSignUpPage->TextBlock_Destination->SetText(FText::FromString(Destination));
}

//perhaps to similar things as the "OnSignUpRequestSucceed()" above
void UUW_SigninOverlay::OnConfirmSignUpRequestSucceed()
{
//clear all EditableTextBoxes in WBP_3:
	WBP_ConfirmSignUpPage->ClearTextBoxes();
//jump to WBP_4:
	ShowSuccessConfirmSignUpPage();
}

//NEXT chapter: stephen didn't have this chain at all
void UUW_SigninOverlay::OnSignInRequestSucceed()
{
//the best place to help to set things to ULocalPlayer::DSSubsystem:
	ULocalPlayerSubsystem_DS* LocalPlayerSubsystem_DS = GetLocalPlayerSubsystem_DS();
	if (IsValid(LocalPlayerSubsystem_DS) == false) return;
	
	LocalPlayerSubsystem_DS->CacheDataToSubsystem( RequestManager_Portal->LastDSSignIn.AuthenticationResult, RequestManager_Portal, RequestManager_Portal->Username_SignIn, RequestManager_Portal->Email_SignIn);

	//this solved puzzle: how to bind RM_Portal::delegate without needing to cast to it lol
	RequestManager_Portal->RefreshTokensRequestSucceedDelegate.AddDynamic(LocalPlayerSubsystem_DS, &ULocalPlayerSubsystem_DS::OnRefreshTokensRequestSucceed);

//warning: you can't call PortalHUD::OnSignIn in here, because it will try to destroy this hosting 'UUW_SigninOverlay' that currently calling this very function lol. all functions are gone, if the hosting object is gone right? but perhaps you can do it "FINAL" in the PortalHUD::OnSignIn
//solution1: call it after "OnResponse_SignIn::SignInRequestSucceedDelegate.Broadcast();" - that will in fact trigger the whole delegate chain, i.e even after this "UUW_SigninOverlay::OnSignInRequestSucceed()" ! yeah! - I talk about this logic many time: "delegate chain" always execute before the next code.
//solution2: set Visibility of WBP_SignInOverlay to "Invisible", set timer to destroy itself. or pass in "this" to be destroyed in PortalHUD::SignIn
//solution3: just use UPROPERTY() URequestManager_Portal* to keep it alive in DSSusbstem lol. because I did use delegate to the main one-way dependency already, what bother you to let PortalManager gone lol!
	
}

//why we can't make the function static? well because UObject::GetWorld() is different when call on different HOSTING object lol
ULocalPlayerSubsystem_DS* UUW_SigninOverlay::GetLocalPlayerSubsystem_DS() const
{
	if (GetWorld() == nullptr) return nullptr;
	
//get FirstLocalPlayer
	ULocalPlayer* FirstLocalPlayer = GetWorld()->GetFirstLocalPlayerFromController(); //or GEngine->GetFirstGamePlayer(GetWorld());
	if (IsValid(FirstLocalPlayer))
	{
		return FirstLocalPlayer->GetSubsystem<ULocalPlayerSubsystem_DS>();	
	}
	return nullptr;;
}



/*To WBP_NewOverlay soon -- better idea is to bind directly to WBP_Sub::helper!
void UUW_SigninOverlay::OnStatusMessageDelegateBroadcast(const FString& StatusMessage, bool bResetWidgetButtons)
{
	WBP_JoinGame->SetStatusMessage(StatusMessage);
	//I decide to do it first this time:
	if(bResetWidgetButtons)
	{
		WBP_JoinGame->Button_JoinGame->SetIsEnabled(true);
	}
}
*/

/*To WBP_NewOverlay soon	
void UUW_SigninOverlay::OnJoinGameButtonClicked()
{
//side steps: I decide to do it first this time:
	WBP_JoinGame->Button_JoinGame->SetIsEnabled(false);

//main steps:
	check(RequestManager_GameSessions)
	RequestManager_GameSessions->StatusMessageDelegate.AddDynamic(this, &ThisClass::OnStatusMessageDelegateBroadcast); 
	RequestManager_GameSessions->SendRequest_FindAndJoinGameSession(); //I add "SendRequest_" so that it is more consistent.
}
*/