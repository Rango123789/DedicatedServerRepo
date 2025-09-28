// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/RequestManager/RequestManager_GameSessions.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/DA_APIInfo.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LocalPlayerSubsystem_DS.h"
#include "UI/RequestManager/HTTPRequestTypes.h"

//STAGE1: Find if any ACTIVE session exists, otherwise create a new on
void URequestManager_GameSessions::SendRequest_FindAndJoinGameSession()
{
	StatusMessageDelegate.Broadcast("Searching for Game Session...", false);

	//Step1: Create an EMPTY request:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	//Step2: bind a callback to execute when "response" reach back:
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnResponse_FindOrCreateGameSession);
	
	//Step3: SetURL,SetVerb,SetHeader for the HttpRequest:
	FString InvokeURL = DA_APIInfo->GetInvokeURLByResourceTag(DedicatedServersTags::GameSessionsAPI::FindOrCreateGameSession);
	HttpRequest->SetURL(InvokeURL);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");

	//step3B: adapt to satisfy Authorizer::{FromCognito::UserPool_i && HeaderToTakeSource="authorization"  + "custom_scope"}
	if(GetLocalPlayerSubsystem_DS())
	{
		//"Authorization" is the HeaderNameContainTokenSource we specified from APIGateway::Authorizer to be set for ::method_i, you can name it differently if you want (but must match each other)
		HttpRequest->SetHeader("Authorization", GetLocalPlayerSubsystem_DS()->GetAccessToken());	
	}
	
	//Step4: actually send it (Unreal API know how to access our OS system or it has browser/framework itself to do it at higher level so don't worry):
	HttpRequest->ProcessRequest();
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("FindOrCreateGameSession request sent"), false);
}

void URequestManager_GameSessions::OnResponse_FindOrCreateGameSession(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                                bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("FindOrCreateGameSession response received"), false);
	
//STAGE0: return early if you can, stephen didn't have this on in TestAPI
	if (bWasSuccessful == false) //this can be merged with "STAGE1::step2
	{
		StatusMessageDelegate.Broadcast("Something went wrong", true);
		return;
	}

/***STAGE1: response->reponse's playload->C++ JsonObject*/
//step0: ready stuff
	TSharedPtr<FJsonObject> JsonObject;
	FString JsonString = Response.Get()->GetContentAsString();
//step1: ready JsonReader = f(JsonString)
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
//step2: Deserialize JsonReader(JsonString) onto JsonObject
	if (FJsonSerializer::Deserialize( JsonReader, JsonObject) == false)
	{
		StatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

/***STAGE2: use the JsonObject:*/
//step1: ERROR checking: (can simply copy, or to be exact, we did factorize the function on ParentManager to reuse it lol)
	if (CheckErrors(JsonObject)) //this can be merged with "STAGE1::step2
	{
		StatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}
//step2: Dump log for "$metadata" sub object: 
	//currently, I didn't include the "$metadata" in lambda return lol, so this won't find the field, and so be doing nothing:
	DumpMetadata(JsonObject);
//step3: JsonObject -> FMatchingStruct/FSubMatchingStruct , and then broadcast it back to be handled in the associate WBP_Overlay
	FDSGameSession DSGameSession;
	FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DSGameSession);
	DSGameSession.Dump();

/***STAGE3: Continue to send HttpRequest2 to CreatePlayerSession and actually join the game: - kind of looping process lol*/
	//factorize are optional, theoretically you must past in (::GameSessionId , "PlayerId", extra..) but "PlayerId" doesn't depend on this HOSTING function so yeah:
	HandleCreatePlayerSession(DSGameSession.GameSessionId, DSGameSession.Status);
}

//STAGE2: create PlayerSession on that found or new session::
void URequestManager_GameSessions::SendRequest_CreatePlayerSession(const FString& GameSessionId, const FString& PlayerId)
{
	StatusMessageDelegate.Broadcast("Creating Player Session..", false);

//Step1: Create an EMPTY request:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
//Step2: bind a callback to execute when "response" reach back:
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnResponse_CreatePlayerSession);
	
//Step3: SetURL,SetVerb,SetHeader for the HttpRequest:
	FString InvokeURL = DA_APIInfo->GetInvokeURLByResourceTag(DedicatedServersTags::GameSessionsAPI::CreatePlayerSession);
	HttpRequest->SetURL(InvokeURL);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");

//step3B: we need SetContent in this case, we can event make a re-usable function for this in GENERIC case!
	//ready JsonObject and set fields and values on it:
	/* NEW code: use my helper function
	FString JsonString;
		//style1:
	TMap<FString, FString> ContentMap;
	ContentMap.Add("GameSessionId", GameSessionId);
	ContentMap.Add("PlayerId", PlayerId);
		//style2: PREFERRED by stephen
	TMap<FString, FString> ContentMap1 =
	{
	{"GameSessionId", GameSessionId},
	{"PlayerId", PlayerId}
	};
	MapToJsonString(ContentMap, JsonString); */
	/*OLD code: I prefer to keep this for this OnResponse_X so that we understand the whole picture! next time I use NEW code*/
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>(); //OR MakeShareable(new FJsonObject())
		//it is important that the SetXField("FieldName" , Value) must match the "event.FieldName" you enter in your lambda function code (also, the key in Test_Event must match "event.FieldName" too for testing nothing more):
	JsonObject->SetStringField("GameSessionId", GameSessionId);
	JsonObject->SetStringField("PlayerId", PlayerId);

	//Convert this JsonObject into JsonString:
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString); //JsonString is linked after this
	FJsonSerializer::Serialize(JsonObject.ToSharedRef() , JsonWriter); //	FJsonObjectConverter::XToY won't work
	//Now set Content for our HttpRequest with that JsonString:
	HttpRequest->SetContentAsString(JsonString);
	
//Step4: actually send it (Unreal API know how to access our OS system, or it has a browser/framework itself to do it at higher level so don't worry):
	HttpRequest->ProcessRequest();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("CreatePlayerSession request sent"), false);	
}

void URequestManager_GameSessions::OnResponse_CreatePlayerSession(TSharedPtr<IHttpRequest> HttpRequest,
	TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("CreatePlayerSession response received"), false);

//step0: return early if our request didn't even reach the server
	if (bWasSuccessful == false) //this can be merged with "STAGE1::step2
	{
		StatusMessageDelegate.Broadcast("Something went wrong", true);
		return;
	}

//step1: get the PayloadString and convert to PlayLoadJsonObject 
	TSharedPtr<FJsonObject> JsonObject;
	bool bConvertSucceed = JsonStringToJsonObject(HttpResponse->GetContentAsString(), JsonObject);
	if (bConvertSucceed ==false)
	{
		StatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

//step2: check if any error in the response:
	if (CheckErrors(JsonObject)) //this can be merged with "STAGE1::step2
	{
		StatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}

//step3: JsonObject -> FMatchingStruct/FSubMatchingStruct, (broadcast it back to be handled in the associate WBP_Overlay if needed)

	FDSPlayerSession DSPlayerSession;
	bool bConvertSucceed2 = FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DSPlayerSession);
	DSPlayerSession.Dump();
	//you can assume that if it reaches step3+debugging it must be fine, but I love to do this, why not :)
	if (bConvertSucceed2 ==false)
	{
		StatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
		return;
	}
	
//step4: [possible step] send another request if needed
	
//step5: [possible step] enable back the button. at this point, after achieve what we want we reset the button anyway? NO NEED here, we will travel travel to the new map anyway lol
	StatusMessageDelegate.Broadcast(TEXT("PlayerSession created!"), true);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("PlayerSession created!"), false);

//for now: travel to server's map:
	//always remember when Client.exe about to travel to new map (even to Server's map) is to "change its input mode":
	if (APlayerController* FirstLocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld()) )
	{
		FInputModeGameOnly InputModeGameOnly;
		FirstLocalPlayerController->SetInputMode(InputModeGameOnly);
		FirstLocalPlayerController->SetShowMouseCursor(true);
	}
	
	//LevelName = IpAddress:Port (used in this source) or Path/to/level.umap (not used in this course)
	FString LevelName = DSPlayerSession.IpAddress + ":" + FString::FromInt(DSPlayerSession.Port);

	//make it ?"Username" or ?"PlayerId" both okay, as long as you search the same key in GM::PreLogin( , , Options)!
	FString Options ="?PlayerSessionId=" + DSPlayerSession.PlayerSessionId +  "?Username=" + DSPlayerSession.PlayerId; //UPDATE
	
	UGameplayStatics::OpenLevel(this, FName(*LevelName), true, Options);
	
}

void URequestManager_GameSessions::HandleCreatePlayerSession(const FString& GameSessionId, const FString& Status)
{
	//the found/created GameSession is LUCKILY active right just after created, or we find an existing ACTIVE session (created by someone else (40%) or by yourself in the next try (40%) or this try (RARE)) then call TryCreatePlayerSession
	if (Status == "ACTIVE")
	{
		StatusMessageDelegate.Broadcast("Find ACTIVE GameSession, Trying to create PlayerSession...", false);

		//Login is before Dashboard level hence at this time you should already have DSSystem::Username stored!
		if (GetLocalPlayerSubsystem_DS())
		{
				//SendRequest_CreatePlayerSession(GameSessionId , GetUniqueIdFromPlayerState());
			SendRequest_CreatePlayerSession(GameSessionId , GetLocalPlayerSubsystem_DS()->Username);	//UPDATE
		}
	}
	/*it is ACTIVATING, then we have many possible option, but the best option after analyzation is to call "SendRequest_FindOrCreateGameSession" (start the whole chain again) but this time with the hope that you will "Find" the created GameSession that just turn to "ACTIVE" from "ACTIVATING". In case it "Find" other ACTIVE GameSession that is not created by you, then it is fine too :D :D. OPTIONALLY you can put it inside a timer if you want. Reason: sending request to AWS have DelayTime itself:
	-PTION1: do it again, knowing that there the newly-created GameSession is activated soon 
	 SetTimer(callback=SendRequest_FindOrCreateGameSession) = this is the best option, regardless the fact that somebody else may take up your slot lol, but you can still create new one it that worst case, and it is totally acceptable! yeah, so this is the final option
	 -OPTION2: use DelegateTimer to pass in the 2 params of this hosting function again
	 SetTimer(TryCreatePlayerSession) = you can't be sure it will be ACTIVE this time right lol? who know it is still ACTIVATING lol
	 -OPTION3: 
	 HandleX( , ); = this won't work because  GameSession.Status is actually changed from Activating to Active lol, nothing help the param to update itself at all :D 
	*/
	else if (Status == "ACTIVATING")
	{
		//no need to broadcast anything in this case, because we retry the whole chain again that will come with new messages
		
		//option1: may work but can be risky if the Delay of re-sending request isn't long enough.
			// SendRequest_FindAndJoinGameSession();
		//option2: GetWorld() only succeed if you set the Owner for this RequestManager to "some appropriate object" that can go outer and find it - it is currently WBP_Overlay, that is CreateWidget and AddToViewport already, WBP_Overlay can in turn find the world! yeah!
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle,  this, &ThisClass::SendRequest_FindAndJoinGameSession, 1.f);
		}
	}
	//The rest could be "TERMINATED" or else. This only happens when the server is down, I believe
	else
	{
		StatusMessageDelegate.Broadcast(HttpStatusMessage::SomethingWentWrong, true);
	}
}
