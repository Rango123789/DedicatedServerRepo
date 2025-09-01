// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDAndOverlay/Portal/UW_SigninOverlay.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "UI/RequestManager/RequestManager_GameSessions.h"
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
}

void UUW_SigninOverlay::OnSignInButtonClicked()
{
	//must take from the right page (SignInPage, not SignUpPage also have those fields lol)
	FText UserName = WBP_SignInPage->EditableTextBox_UserName->GetText();
	FText Password = WBP_SignInPage->EditableTextBox_Password->GetText();
	RequestManager_Portal->SignIn(UserName.ToString(), Password.ToString());
}

void UUW_SigninOverlay::OnSignUpButtonClicked()
{
	//the same logic here:
	FText UserName = WBP_SignUpPage->EditableTextBox_UserName->GetText();
	FText Email = WBP_SignUpPage->EditableTextBox_Email->GetText();
	FText Password = WBP_SignUpPage->EditableTextBox_Password->GetText();
	FText ConfirmPassword = WBP_SignUpPage->EditableTextBox_ConfirmPassword->GetText();
	RequestManager_Portal->SignUp(UserName.ToString(), Email.ToString(), Password.ToString(), ConfirmPassword.ToString());
}

void UUW_SigninOverlay::OnConfirmButtonClicked()
{
	FText VerificationCode = WBP_ConfirmSignUpPage->EditableTextBox_VerificationCode->GetText();
	RequestManager_Portal->Confirm(VerificationCode.ToString());
}

void UUW_SigninOverlay::ShowSignInPage()
{
	WidgetSwitcher->SetActiveWidget(WBP_SignInPage);
}
void UUW_SigninOverlay::ShowSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(WBP_SignUpPage);
}

void UUW_SigninOverlay::ShowConfirmSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(WBP_ConfirmSignUpPage);
}

void UUW_SigninOverlay::ShowSuccessConfirmSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(WBP_SuccessConfirmSignUpPage);
}

/*To WBP_NewOverlay soon	
void UUW_SigninOverlay::OnStatusMessageDelegateBroadcast(const FString& StatusMessage, bool bResetJoinGameButton)
{
	WBP_JoinGame->SetStatusMessage(StatusMessage);
	//I decide to do it first this time:
	if(bResetJoinGameButton)
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