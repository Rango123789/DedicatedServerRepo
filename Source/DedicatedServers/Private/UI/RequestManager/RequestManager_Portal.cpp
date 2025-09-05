// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RequestManager/RequestManager_Portal.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/DA_APIInfo.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpResponse.h"
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

//we don't need confirm password here, because we already handle it at first place in WBP_XOverlay::WBP_SignUpPage (SignUp button only enable when patterns are VALID)! However for now I like to keep it for fun lol:
void URequestManager_Portal::SendRequest_SignUp(const FString& Username, const FString& Email, const FString& Password,
	const FString& ConfirmPassword)
{
	SignUpStatusMessageDelegate.Broadcast("Creating a new account...", false);

//Step1: Create an EMPTY request:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
//Step2: bind a callback to execute when "response" reach back:
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnResponse_SignUp);
	
//Step3: SetURL,SetVerb,SetHeader for the HttpRequest:
	FString InvokeURL = DA_APIInfo->GetInvokeURLByResourceTag(DedicatedServersTags::PortalAPI::SignUp);
	HttpRequest->SetURL(InvokeURL);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");

//step3B: we need SetContent in this case, we can event make a re-usable function for this in GENERIC case!
	//ready JsonObject and set fields and values on it:
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>(); //OR MakeShareable(new FJsonObject())
	JsonObject->SetStringField("Username", Username);
	JsonObject->SetStringField("Email", Email);
	JsonObject->SetStringField("Password", Password);

	LastUsername_SignUp = Username; 

	//Convert this JsonObject into JsonString:
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString); //JsonString is linked after this
	FJsonSerializer::Serialize(JsonObject.ToSharedRef() , JsonWriter); //	FJsonObjectConverter::XToY won't work
	//Now set Content for our HttpRequest with that JsonString:
	HttpRequest->SetContentAsString(JsonString);
	
//Step4: actually send it (Unreal API know how to access our OS system, or it has a browser/framework itself to do it at higher level so don't worry):
	HttpRequest->ProcessRequest();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("SignUp request sent"), false);	
}

void URequestManager_Portal::OnResponse_SignUp(TSharedPtr<IHttpRequest> HttpRequest,
	TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("SignUp response received"), false);

//step0: return early if our request didn't even reach the server
	if (bWasSuccessful == false) //this can be merged with "STAGE1::step2
	{
		SignUpStatusMessageDelegate.Broadcast("Something went wrong", true);
		return;
	}

//step1: get the PayloadString and convert to PlayLoadJsonObject 
	TSharedPtr<FJsonObject> JsonObject;
	bool bConvertSucceed = JsonStringToJsonObject(HttpResponse->GetContentAsString(), JsonObject);
	if (bConvertSucceed ==false)
	{
		SignUpStatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

//step2: check if any error in the response:
	if (CheckErrors(JsonObject)) //this can be merged with "STAGE1::step2
	{
		SignUpStatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

//step3: JsonObject -> FMatchingStruct/FSubMatchingStruct, (broadcast it back to be handled in the associate WBP_Overlay if needed)

	FDSSignUp DSSignUp;
	bool bConvertSucceed2 = FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DSSignUp);
	DSSignUp.Dump();
	
	//you can assume that if it reaches step3+debugging it must be fine, but I love to do this, why not :)
	if (bConvertSucceed2 ==false)
	{
		SignUpStatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

	//I only set LastDSSignUp when it succeed:
	LastDSSignUp = DSSignUp;
	
//step4: [possible step] send another request if needed.
	//HERE say jump on WBP_Confirm and send SendRequest_Confirm?
	//=HELL NO we did setup to make it auto-jump to that WBP_3 already and then the 'Confirm' button will do that job INDEPENDENTLY lol
	//=so we're almost done here! yeah!

//step5: [possible step] broadcast all relevant delegates (enable back the button and so on). HERE we will jump into WBP_ConfirmSignUpPage anyway
	//STEPHEN didn't add this (*), because indeed it won't be seen when we switch to WBP_3 lol, but still it is important in "SomethingWentWrong" case - but I suggest we clear WBP_i::TextBlock as well (in WBP_i::ClearBoxes) when we move to WBP_i+1 (not just ::TextBoxes)
		//SignInStatusMessageDelegate.Broadcast(TEXT("Signed Up, waiting for confirmation..."), true); //(*)
	SignUpRequestSucceedDelegate.Broadcast();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("SignUp done, waiting for confirmation!"), false);	
}

void URequestManager_Portal::SendRequest_ConfirmSignUp(const FString& ConfirmationCode)
{
	//SignInStatusMessageDelegate.Broadcast("Confirming the new account...", false); //me
	ConfirmSignUpStatusMessageDelegate.Broadcast("Checking confirmation code...", false); //stephen
//Step1: Create an EMPTY request:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
//Step2: bind a callback to execute when "response" reach back:
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnResponse_ConfirmSignUp);
	
//Step3: SetURL,SetVerb,SetHeader for the HttpRequest:
	FString InvokeURL = DA_APIInfo->GetInvokeURLByResourceTag(DedicatedServersTags::PortalAPI::ConfirmSignUp);
	HttpRequest->SetURL(InvokeURL);
	HttpRequest->SetVerb("PUT"); //PUT, not POST any more (the state has been created)
	HttpRequest->SetHeader("Content-Type", "application/json");

//step3B: we need SetContent in this case, we can event make a re-usable function for this in GENERIC case!
	//ready JsonObject and set fields and values on it:
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>(); //OR MakeShareable(new FJsonObject())

	//there is no VerificationCode field in AWS lol, there is "ConfirmationCode"
	JsonObject->SetStringField("ConfirmationCode", ConfirmationCode);

	//you may forget this field, it is required for ConfirmSignUp command. Of course if don't have Username or other alias, how Cognito know which one to confirm lol.
	//the DOC "Username" can be Username or alias (like "UserSub" or other acceptable alias). If we don't want to store Username from WBP_2, we can simply access the alternative "UserSub" (not Username that is much shorter (also need to be unique in the current user pool that check "Username" as "SignIn Option"), "UserSub"is like the User Id). Stephen in fact store LastUsername lol.
	
	//SHOCKING NEWS: using LastDSSignUp.UserSub give me "ExpiredCodeException" (meaning it still accept the request but behave wierdly)...where I try "LastUserName directly it works!
	//JsonObject->SetStringField("Username", LastDSSignUp.UserSub);  //Cognito allow to use UserSub instead of "Username" (if it isn't available)
	JsonObject->SetStringField("Username", LastUsername_SignUp); 
	//Convert this JsonObject into JsonString:
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString); //JsonString is linked after this
	FJsonSerializer::Serialize(JsonObject.ToSharedRef() , JsonWriter); //	FJsonObjectConverter::XToY won't work
	//Now set Content for our HttpRequest with that JsonString:
	HttpRequest->SetContentAsString(JsonString);
	
//Step4: actually send it (Unreal API know how to access our OS system, or it has a browser/framework itself to do it at higher level so don't worry):
	HttpRequest->ProcessRequest();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("ConfirmSignUp request sent"), false);	
}

//high chance we will create a new StatusMessageDelegate/like or we need to "bind and unbind" (perhaps bind when OnClicked and unbind in OnResponse_X)
//yes stephen create one delegate for each lol! cost more memory but we don't worry about "bind and unbind" any more, which is stupid and time-consuming
void URequestManager_Portal::OnResponse_ConfirmSignUp(TSharedPtr<IHttpRequest> HttpRequest,
	TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful)
{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("ConfirmSignUp response received"), false);

//step0: return early if our request didn't even reach the server
	if (bWasSuccessful == false) //this can be merged with "STAGE1::step2
	{
		ConfirmSignUpStatusMessageDelegate.Broadcast("Something went wrong", true);
		return;
	}

//step1: get the PayloadString and convert to PlayLoadJsonObject 
	TSharedPtr<FJsonObject> JsonObject;
	bool bConvertSucceed = JsonStringToJsonObject(HttpResponse->GetContentAsString(), JsonObject);
	if (bConvertSucceed ==false)
	{
		ConfirmSignUpStatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

//step2: check if any error in the response:
	if (CheckErrors(JsonObject)) //this can be merged with "STAGE1::step2
	{
		//ADAPT this one a bit in this OnReponse_X only, to make it better for players:
		//now do we know field "name" to test? enter a wrong code, debug the "JsonObject" and we will see "name" : "CodeMismatchException"
		if (JsonObject->HasField(TEXT("name")) && JsonObject->GetStringField(TEXT("name")) == "CodeMismatchException")
		{
			ConfirmSignUpStatusMessageDelegate.Broadcast("invalid code", true);		
		}
		else //if not just give "GENERIC error message" lol
		{
			ConfirmSignUpStatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);		
		}
		
		return;
	}

//step3: JsonObject -> FMatchingStruct/FSubMatchingStruct, (broadcast it back to be handled in the associate WBP_Overlay if needed)

	/* the response of ConfirmSignUp is empty (except $metadata that can be handled by CheckErrors already)
	FDSSignUp DSSignUp;
	bool bConvertSucceed2 = FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DSSignUp);
	DSSignUp.Dump();
	
	//you can assume that if it reaches step3+debugging it must be fine, but I love to do this, why not :)
	if (bConvertSucceed2 ==false)
	{
		StatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

	//I only set LastDSSignUp when it succeed:
	LastDSSignUp = DSSignUp;

	*/
	
//step4: [possible step] send another request if needed.
	//HERE say jump on WBP_Confirm and send SendRequest_Confirm?
	//=HELL NO we did setup to make it auto-jump to that WBP_3 already and then the 'Confirm' button will do that job INDEPENDENTLY lol
	//=so we're almost done here! yeah!

//step5: [possible step] broadcast all relevant delegates (enable back the button and so on). HERE we will jump into WBP_ConfirmSignUpPage anyway
	//PUZZLE: we may want to set WBP_3::TextBlock_StatusMessage, also bind its callback to this same delegate lol? it will set all WBP_1,2,3::TextBlock_StatusMessage to the lastest broadcast's values (WBP_4 doesn't have TextBlock_StatusMessage, WBP_1 has it and it should be used soon) and could leave artifact when we come back to them. So "bind" and "unbind" now become "A MUST" if we want to reuse this same delegate lol :)
	//STEPHEN didn't add this (*), because indeed it won't be seen when we switch to WBP_4 lol, and we will see the message in WBP_4 anyway, but still it is important in "SomethingWentWrong" case - but I suggest we clear WBP_i::TextBlock (in WBP_i::ClearBoxes) as well when we move to WBP_i+1 (not just ::TextBoxes)
		//ConfirmSignInStatusMessageDelegate.Broadcast(TEXT("The new account has been confirmed!"), true); //(*)
	ConfirmSignUpRequestSucceedDelegate.Broadcast();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("The new account has been confirmed! You now can log in"), false);	
}

void URequestManager_Portal::SendRequest_SignIn(const FString& Username, const FString& Password)
{
	SignInStatusMessageDelegate.Broadcast("Signing in...", false); //stephen
//Step1: Create an EMPTY request:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
//Step2: bind a callback to execute when "response" reach back:
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnResponse_SignIn);
	
//Step3: SetURL,SetVerb,SetHeader for the HttpRequest:
	FString InvokeURL = DA_APIInfo->GetInvokeURLByResourceTag(DedicatedServersTags::PortalAPI::SignIn);
	HttpRequest->SetURL(InvokeURL);

	//shocking news: we thought it is "GET", but as long as we need to send the request with Body ({JsonString}) we need "POST". No why, this is conventional lol.
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");

//step3B: we need SetContent in this case, we can event make a re-usable function for this in GENERIC case!
	//ready JsonObject and set fields and values on it:
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>(); //OR MakeShareable(new FJsonObject())

	//there is no VerificationCode field in AWS lol, there is "ConfirmationCode"
	JsonObject->SetStringField("Username", Username); 
	JsonObject->SetStringField("Password", Password);
	LastUsername_SignIn = Username;
	
	//Convert this JsonObject into JsonString:
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString); //JsonString is linked after this
	FJsonSerializer::Serialize(JsonObject.ToSharedRef() , JsonWriter); //	FJsonObjectConverter::XToY won't work
	//Now set Content for our HttpRequest with that JsonString:
	HttpRequest->SetContentAsString(JsonString);
	
//Step4: actually send it (Unreal API know how to access our OS system, or it has a browser/framework itself to do it at higher level so don't worry):
	HttpRequest->ProcessRequest();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("SignIn request sent"), false);
}

void URequestManager_Portal::OnResponse_SignIn(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("SignIn response received"), false);

//step0: return early if our request didn't even reach the server
	if (bWasSuccessful == false) //this can be merged with "STAGE1::step2
	{
		SignInStatusMessageDelegate.Broadcast("Something went wrong", true);
		return;
	}

//step1: get the PayloadString and convert to PlayLoadJsonObject 
	TSharedPtr<FJsonObject> JsonObject;
	bool bConvertSucceed = JsonStringToJsonObject(HttpResponse->GetContentAsString(), JsonObject);
	if (bConvertSucceed ==false)
	{
		SignInStatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

//step2: check if any error in the response:
	if (CheckErrors(JsonObject)) //this can be merged with "STAGE1::step2
	{
		SignInStatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);		
		return;
	}

//step3: JsonObject -> FMatchingStruct/FSubMatchingStruct, (broadcast it back to be handled in the associate WBP_Overlay if needed)

	FDSSignIn DSSignIn;
	bool bConvertSucceed2 = FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DSSignIn);
	DSSignIn.Dump();
	
	//you can assume that if it reaches step3+debugging it must be fine, but I love to do this, why not :)
	if (bConvertSucceed2 ==false)
	{
		SignInStatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

	LastDSSignIn = DSSignIn;
//step4: [possible step] send another request if needed.

//step5: [possible step] broadcast all relevant delegates (enable back the button and so on). 
	SignInStatusMessageDelegate.Broadcast(TEXT("Sign in successfully!"), true); //(*)
	SignInRequestSucceedDelegate.Broadcast();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("Sign in successfully!"), false);	
}



//WORKING ON: no longer need broadcast any StatusMessageDelegate
void URequestManager_Portal::SendRequest_RefreshTokens(const FString& Username, const FString& RefreshToken)
{
//Step1: Create an EMPTY request:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
//Step2: bind a callback to execute when "response" reach back:
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnResponse_RefreshTokens);
	
//Step3: SetURL,SetVerb,SetHeader for the HttpRequest:
	FString InvokeURL = DA_APIInfo->GetInvokeURLByResourceTag(DedicatedServersTags::PortalAPI::SignIn);
	HttpRequest->SetURL(InvokeURL);

	//shocking news: we thought it is "GET", but as long as we need to send the request with Body ({JsonString}) we need "POST". No why, this is conventional lol.
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");

//step3B: we need SetContent in this case, we can event make a re-usable function for this in GENERIC case!
	//ready JsonObject and set fields and values on it:
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>(); //OR MakeShareable(new FJsonObject())

	//there is no VerificationCode field in AWS lol, there is "ConfirmationCode"
	JsonObject->SetStringField("Username", Username); 
	JsonObject->SetStringField("RefreshToken", RefreshToken);
	
	//Convert this JsonObject into JsonString:
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString); //JsonString is linked after this
	FJsonSerializer::Serialize(JsonObject.ToSharedRef() , JsonWriter); //	FJsonObjectConverter::XToY won't work
	//Now set Content for our HttpRequest with that JsonString:
	HttpRequest->SetContentAsString(JsonString);
	
//Step4: actually send it (Unreal API know how to access our OS system, or it has a browser/framework itself to do it at higher level so don't worry):
	HttpRequest->ProcessRequest();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("RefreshTokens request sent"), false);
}

//WORKING ON: no longer need broadcast any StatusMessageDelegate
void URequestManager_Portal::OnResponse_RefreshTokens(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("RefreshTokens response received"), false);

//step0: return early if our request didn't even reach the server
	if (bWasSuccessful == false) //this can be merged with "STAGE1::step2
	{
		return;
	}

//step1: get the PayloadString and convert to PlayLoadJsonObject 
	TSharedPtr<FJsonObject> JsonObject;
	bool bConvertSucceed = JsonStringToJsonObject(HttpResponse->GetContentAsString(), JsonObject);
	if (bConvertSucceed ==false)
	{
		return;
	}

//step2: check if any error in the response:
	if (CheckErrors(JsonObject)) //this can be merged with "STAGE1::step2
	{
		return;
	}

//step3: JsonObject -> FMatchingStruct/FSubMatchingStruct, (broadcast it back to be handled in the associate WBP_Overlay if needed)
	FDSSignIn DSSignIn;
	bool bConvertSucceed2 = FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DSSignIn);
	//DSSignIn.Dump();
	DSSignIn.AuthenticationResult.Dump();
	
	//you can assume that if it reaches step3+debugging it must be fine, but I love to do this, why not :)
	if (bConvertSucceed2 ==false)
	{
		return;
	}

	/*I assume RefreshToken isn't changed, where Access,ID token change? not only it is not changed, but it is not included lol
	SHOCKING NEWS: the return of FAuthenticationResult in
		(1) SignIn mode = include "fresh token"
		(2) SignIn_RefreshToken mode = NOT include "fresh token" (include but "EMPTY")
		, hence DO NOT let DSSignIn to override LastDSSignIn completely lol!
	*/
	LastDSSignIn.AuthenticationResult.AccessToken = DSSignIn.AuthenticationResult.AccessToken;
	LastDSSignIn.AuthenticationResult.IdToken = DSSignIn.AuthenticationResult.IdToken;
	
//step4: [possible step] send another request if needed.

//step5: [possible step] broadcast all relevant delegates (enable back the button and so on).
	RefreshTokensRequestSucceedDelegate.Broadcast(LastUsername_SignIn, LastDSSignIn.AuthenticationResult);

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("RefreshTokens successfully!"), false);	
}


















