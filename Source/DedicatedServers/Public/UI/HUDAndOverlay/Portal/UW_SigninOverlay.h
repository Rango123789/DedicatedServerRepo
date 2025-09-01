// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SigninOverlay.generated.h"

class URequestManager_GameSessions;
class UButton;
class UUW_SuccessConfirmSignUpPage;
class UUW_ConfirmSignUpPage;
class UUW_SignUpPage;
class UUW_SignInPage;
class UWidgetSwitcher;
class UUW_JoinGame;
class URequestManager_Portal;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_SigninOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	
protected:
/*To WBP_NewOverlay soon	
	UFUNCTION()
	void OnStatusMessageDelegateBroadcast(const FString& StatusMessage, bool bResetJoinGameButton);
	UFUNCTION()
	void OnJoinGameButtonClicked();
*/

	UFUNCTION()
	void OnSignInButtonClicked();
	UFUNCTION()
	void OnSignUpButtonClicked();
	UFUNCTION()
	void OnConfirmButtonClicked();
	
	virtual void NativeConstruct() override;

//Request Manager:
	/*@@IMPORTANT notes:
     	(1) Manager is added to WBP_HUD help it handle side works about HTTP requesting and receiving
     	, not added to HUD in this course lol!
     	(2) in GAS course we did create all WC_X,Y,Z and add them all to HUD level
     	, so that we make it possible they can be accessible anywhere if needed (not the case in this course so yeah)*/
     UPROPERTY(EditAnywhere)
	TSubclassOf<URequestManager_Portal> RequestManager_Portal_Class;
	UPROPERTY()
	TObjectPtr<URequestManager_Portal> RequestManager_Portal;

/*To WBP_NewOverlay soon	
	UPROPERTY(EditAnywhere)
	TSubclassOf<URequestManager_Portal> RequestManager_GameSessions_Class;
	UPROPERTY()
	TObjectPtr<URequestManager_GameSessions> RequestManager_GameSessions;
*/

/*To WBP_NewOverlay soon	
//this one current one WBP_sub of WBP_SignInOverlay lol, but LATER supposedly it will only be dynamically spawned when we pass the log-in process: you're almost right! it will be in WBP_NewOverlay!
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UUW_JoinGame> WBP_JoinGame;
*/

//the UWidgetSwitcher::{WBP_1,2,3,4} part: (OPTION2: WBP_Switcher and WBP_Switcher handle those {WBP_1,2,3,4} locally!)
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UUW_SignInPage> WBP_SignInPage;

	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UUW_SignUpPage> WBP_SignUpPage;

	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UUW_ConfirmSignUpPage> WBP_ConfirmSignUpPage;

	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UUW_SuccessConfirmSignUpPage> WBP_SuccessConfirmSignUpPage;
	
	//for debugging purposes:
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UButton> Button_Test_SignInPage;
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UButton> Button_Test_SignUpPage;
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UButton> Button_Test_ConfirmSignUpPage;
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr<UButton> Button_Test_SuccessConfirmSignUpPage;

	//callbacks for those debugging buttons:
	UFUNCTION()
	void ShowSignInPage();
	UFUNCTION()
	void ShowSignUpPage() ;
	UFUNCTION()
	void ShowConfirmSignUpPage();
	UFUNCTION()
	void ShowSuccessConfirmSignUpPage();
};
