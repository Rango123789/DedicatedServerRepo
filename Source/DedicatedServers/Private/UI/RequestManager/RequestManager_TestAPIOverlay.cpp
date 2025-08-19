// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RequestManager/RequestManager_TestAPIOverlay.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/DA_APIInfo.h"
#include "DedicatedServers/DedicatedServers.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpResponse.h"
#include "UI/RequestManager/HTTPRequestTypes.h"

void URequestManager_TestAPIOverlay::OnClicked_ListFleetsButton()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("OnClicked_ListFleets"), false);

//Step1: Create an EMPTY request:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

//Step2: bind a callback to execute when "response" reach back:
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnResponse_ListFleets);

//Step3: SetURL,SetVerb,SetHeader for the HttpRequest
	FString InvokeURL = DA_APIInfo_GameSessions->GetInvokeURLByResourceTag(DedicatedServersTags::GameSessionsAPI::ListFleets);
	HttpRequest->SetURL(InvokeURL);

	HttpRequest->SetVerb(TEXT("GET")); //it defaults to GET if you don't set anyway

	/*
	 1. Why we specify "Content-Type" but didn't actually, call "HttpRequest->SetContent()" lol?
	[RAN] SetContent() is usually for "POST" (we currently use GET),
	[GPT] What SetHeader("Content-Type", ...) does
	-It just tells the server: “When I eventually send a body, interpret it as JSON.”
	-It doesn’t actually add a body. It’s just metadata about how to decode the payload.

	+HTTP works like this:
	-"Request headers" (HttpRequest->SetHeader) describe "the request" you are sending.
	-"Response headers" come back from the server separately, and you don’t control them with your request.

	+The response’s content type is declared by the server in the response header:
	  Content-Type: application/json (html)

	+And you can read it in Unreal via:
	  FString ResponseContentType = Response->GetHeader("Content-Type");

	+If you want to tell the server what format you expect back - That’s not "Content-Type", it’s the "Accept" header:
		HttpRequest->SetHeader("Accept", "application/json");
	; It means: “Please send me JSON in the response.”

	"Content-Type" → describes the request body format.
	"Accept" → describes the expected response format. -->RELEVANT: while create "API_X::Resource_X::GET we did specify "content type's the response body" to be "application/json" remember? yeah!

	*/
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

//Step4: actually send it (Unreal API know how to access our OS system or it has browser/framework itself to do it at higher level so don't worry):
	HttpRequest->ProcessRequest();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("ListFleets request sent"), false);
}

void URequestManager_TestAPIOverlay::OnResponse_ListFleets(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("ListFleets response received"), false);

//Step0: the Response you will you "Content" (in string):
	FString ContentAsString = Response.Get()->GetContentAsString();

//step1: ready JsonObject (to be exact TSharedPtr<FJsonObject>)	
	//Trick "FJsonSerializer::Deserialize( , JsonObject) -->create local variable:
	TSharedPtr<FJsonObject> JsonObject;

//step2: ready TJsonReader<TCHAR> (to be exact TSharedRef<  TJsonReader<TCHAR> > ) using "ContentAsString" from HttpResponse::GetContentAsString
	//if you don't specify <TCHAR> for both/either of them, it default to "TCHAR" anyway, if you sepcify it will be grayed out, meaning it is 'REDUNDANT':
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ContentAsString);

//step3: deserialize the "JSonReader" (holding ContentAsString) onto "JsonObject".
	FJsonSerializer::Deserialize(JsonReader , JsonObject);

//step4: now you can use JsonObject (with its built-in helpers) to access anything from "ContentAsString" with ease:
	//ERROR checking:
if ( JsonObject->HasField(TEXT("errorType")) ||
	 JsonObject->HasField(TEXT("errorMessage")))
{
	//this is enough I think:
	FString errorType = JsonObject->HasField(TEXT("errorType")) ? JsonObject->GetStringField(TEXT("errorType")) : TEXT("Unknown Error");
	UE_LOG(GameServerLog, Error, TEXT("errorType: %s"), *errorType );
	
	//you don't want this. it has secret information
	FString errorMessage = JsonObject->HasField(TEXT("errorMessage")) ? JsonObject->GetStringField(TEXT("errorMessage")) : TEXT("Unknown Error Message");
	UE_LOG(GameServerLog, Error, TEXT("errorMessage: %s"), *errorMessage ); 
	return; //return early if there is error!
}

if (JsonObject->HasField(TEXT("$fault")))
{
	//this time we look for "name" field that actually contain the error in this case (just direclty look at the lambda test and you will see lol, no why):
	FString errorType = JsonObject->HasField(TEXT("name")) ? JsonObject->GetStringField(TEXT("name")) : TEXT("Unknown Error");
	UE_LOG(GameServerLog, Error, TEXT("errorType: %s"), *errorType );
	return; //return early if there is error!
}

	
	//practice1:
	if (JsonObject->HasField(TEXT("FleetIds"))) //go check if the "key" matches the key from AWS's ShowListFleet function!
	{
		//FJSonObject::GetArrayField return "an array of FJsonValue" (to be exact...), that can in turn access FJsonValue::AsSomething()...
		TArray<TSharedPtr<FJsonValue>> JsonValuePointers = JsonObject->GetArrayField(TEXT("FleetIds"));

		for (TSharedPtr<FJsonValue> JsonValuePointer : JsonValuePointers)
		{
			FString JsonValueAsString = JsonValuePointer->AsString();
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, JsonValueAsString, false);
		}
	}

	//practice2: NOT work, always use "HasField" when use "GetXField" to avoid "RED error" in case not found!
	if(JsonObject->HasField(TEXT("requestId")))
	{
		FString requestId = JsonObject->GetStringField(TEXT("requestId"));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, requestId, false);
	}
	
	if(JsonObject->HasField(TEXT("$metadata")))
	{
		//practice3: work like a charm
		TSharedPtr<FJsonObject> MetadataJsonObject = JsonObject->GetObjectField(TEXT("$metadata"));
		FString requestId_2 = MetadataJsonObject->GetStringField(TEXT("requestId"));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, requestId_2, false);

		//practice4: you still need to get metadata first
		FDSMetadata MetadataStruct;
		FJsonObjectConverter::JsonObjectToUStruct( MetadataJsonObject.ToSharedRef(), &MetadataStruct );
		MetadataStruct.Dump();
	}

	//practice5: no need to check this time (we didn't use any ::GetXField() )
	FDSListFleetsReponse DSListFleetsResponse;
	FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DSListFleetsResponse);
	DSListFleetsResponse.Dump();


}

void URequestManager_TestAPIOverlay::OnResponse_ListFleets_V2(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bWasSuccessful)
{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("ListFleets response"), false);
}
