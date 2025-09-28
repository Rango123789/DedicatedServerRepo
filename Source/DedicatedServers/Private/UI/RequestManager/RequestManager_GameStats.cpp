// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/RequestManager/RequestManager_GameStats.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/DA_APIInfo.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/RequestManager/HTTPRequestTypes.h"

void URequestManager_GameStats::SendRequest_RecordGameStats(const FDSRecordMatchStats& DSRecordGameStats)
{
//Step1: Create an EMPTY request:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
//Step2: bind a callback to execute when "response" reach back:
	//SHOCKING news: this time we don't even need OnReponse_RecordMatchStats
	//stephen: notice that I didn't even buy a call back to this for the response we could if we want to check for errors but we're just going to verify by checking our database if it doesn't work we can fix it right?
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnResponse_RecordGameStats);
	
//Step3: SetURL, SetVerb, SetHeader for the HttpRequest:
	FString InvokeURL = DA_APIInfo->GetInvokeURLByResourceTag(DedicatedServersTags::GameStatsAPI::RecordMatchStats);
	HttpRequest->SetURL(InvokeURL);

	//shocking news: we thought it is "GET", but as long as we need to send the request with Body ({JsonString}) we need "POST". No why, this is conventional lol.
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");

//step3B: we need SetContent in this case, we can event make a re-usable function for this in GENERIC case!
	/*not sure we need "MakeShared<FJsonObject>()" this time or leave it "nullptr"?
	- yes! you need a valid FJsonObject if you are calling FJsonObjectConverter::UStructToJsonObject(...)
	- because that function requires a non-null JsonObject reference where it will write the converted fields.
	- If you pass nullptr, the call will fail (it has nowhere to put the data).*/
	FString JsonString;
	//option1: could it be that using option1 will void it make the first letter lower-case?
		/*TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>(); //OR MakeShareable(new FJsonObject())
		bool bConvertSucceed2 = FJsonObjectConverter::UStructToJsonObject(FDSRecordMatchStats::StaticStruct(), &DSRecordGameStats, JsonObject.ToSharedRef());
		TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString); 
		FJsonSerializer::Serialize(JsonObject.ToSharedRef() , JsonWriter);*/
	//option2: (meaning skip the whole code in option1)
		FJsonObjectConverter::UStructToJsonObjectString(FDSRecordMatchStats::StaticStruct(), &DSRecordGameStats, JsonString);
		HttpRequest->SetContentAsString(JsonString);
	
//Step4: actually send it (Unreal API know how to access our OS system, or it has a browser/framework itself to do it at higher level so don't worry):
	HttpRequest->ProcessRequest();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, JsonString, false);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("RecordGameStats request sent"), false);
}

void URequestManager_GameStats::OnResponse_RecordGameStats(TSharedPtr<IHttpRequest> HttpRequest,
	TSharedPtr<IHttpResponse> HttpResponse, bool bSuccessful)
{
	
}