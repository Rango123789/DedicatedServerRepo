// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RequestManager/RequestManager_Portal.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/DA_APIInfo.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpResponse.h"
#include "UI/RequestManager/HTTPRequestTypes.h"

void URequestManager_Portal::SendRequest_FindAndJoinGameSession()
{
	StatusMessageDelegate.Broadcast("Searching for Game Session...", false);

	//Step1: Create an EMPTY request:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	//Step2: bind a callback to execute when "response" reach back:
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnResponse_FindOrCreateGameSession);
	
	//Step3: SetURL,SetVerb,SetHeader for the HttpRequest:
	FString InvokeURL = DA_APIInfo_GameSessions->GetInvokeURLByResourceTag(DedicatedServersTags::GameSessionsAPI::FindOrCreateGameSession);
	HttpRequest->SetURL(InvokeURL);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");
	
	//Step4: actually send it (Unreal API know how to access our OS system or it has browser/framework itself to do it at higher level so don't worry):
	HttpRequest->ProcessRequest();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("FindOrCreateGameSession request sent"), false);
}

void URequestManager_Portal::OnResponse_FindOrCreateGameSession(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                                bool bWasSuccessful)
{
//STAGE0: return early if you can, stephen didn't have this on in TestAPI
	if (bWasSuccessful == false) //this can be merged with "STAGE1::step2
	{
		StatusMessageDelegate.Broadcast("Something went wrong", true);
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("FindOrCreateGameSession response received"), false);
	
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

	//false because we will continue to send request to CreatePlayerSession now:
	StatusMessageDelegate.Broadcast("Game Session Found!", false);
	
}
