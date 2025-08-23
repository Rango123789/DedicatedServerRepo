// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDAndOverlay/TestAPI/TestAPIOverlay.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "UI/Widgets/API/ListFleets/UW_FleetId.h"
#include "UI/Widgets/API/ListFleets/UW_ListFleetsBox.h"
#include "UI/RequestManager/RequestManager_TestAPIOverlay.h"



void UTestAPIOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(RequestManager_TestAPIOverlay_Class)
	RequestManager_TestAPIOverlay = NewObject<URequestManager_TestAPIOverlay>(this, RequestManager_TestAPIOverlay_Class);

	//you better off not run the game until all WBP_HOST, WBP_Xs having all bound widgets from there
	check(WBP_ListFleetsBox)
	check(WBP_ListFleetsBox->Button_ListFleets)

	//.AddDynamic(ObjectHavingFunction, &TypeOfTheObject::FunctionName)
		//WBP_ListFleetsBox->Button_ListFleets->OnClicked.AddDynamic(RequestManager_TestAPIOverlay, &URequestManager_TestAPIOverlay::SendRequest_ListFleets); //OLD
	WBP_ListFleetsBox->Button_ListFleets->OnClicked.AddDynamic(this, &ThisClass::OnClickedCallback_ListFleets);
}

//to many intermediate callbacks lol
void UTestAPIOverlay::OnClickedCallback_ListFleets()
{
	//UPDATE: don't let users to spam the buttons , let's disable it and enable it back when response received back here: (make sure it will always trigger - broadcast Manager::delegate in any case)
	WBP_ListFleetsBox->Button_ListFleets->SetIsEnabled(false);
	
	//you can bind WBP_TestAPIOverlay::OnResponseReceived() to WBP_TestAPIOverlay::RequestManager_TestAPIOverlay::OnResponseReceivedDelegate in "NativeConstruct" right above or HERE (and so you should/must unbind when in WBP_TestAPIOverlay::OnResponseReceived() for good lol, reason: click many times will trigger the callback many times? I recall so lol!)
	//UPDATE: better off bind this before you "SendRequest" lol (I know it take time to come back but anyway)
	RequestManager_TestAPIOverlay->OnListFleetsResponseReceivedDelegate.AddDynamic(this, &ThisClass::OnListFleetsResponseReceived);
	
	//Will in turn trigger "OnReponse_X" when received reponse and also broadcast the delegate from there.
	RequestManager_TestAPIOverlay->SendRequest_ListFleets();
}

//this gives you an option to do thing when receive response right from WBP_X level :), so that URequestManager don't need to be aware of many things that it needn't/shouldn't!
void UTestAPIOverlay::OnListFleetsResponseReceived(const FDSListFleetsResponse& DSListFleetsReponse, bool bWasSuccessful)
{
	//as I said, if we did it the OnClickedCallback (not NativeStruct trigger once) that trigger per click, then you must/should inbind here too:
	if (RequestManager_TestAPIOverlay->OnListFleetsResponseReceivedDelegate.IsBound()){} //check if anyway
	if (RequestManager_TestAPIOverlay->OnListFleetsResponseReceivedDelegate.IsAlreadyBound(this, &ThisClass::OnListFleetsResponseReceived)) //check specific callback is already bound
	{
		RequestManager_TestAPIOverlay->OnListFleetsResponseReceivedDelegate.Clear(); //all callbacks from all objects 
		RequestManager_TestAPIOverlay->OnListFleetsResponseReceivedDelegate.RemoveAll(this); //all callbacks of this object only

		//only the specified callback of the specifified object: I prefer the later
		RequestManager_TestAPIOverlay->OnListFleetsResponseReceivedDelegate.Remove(this, "OnListFleetsResponseReceived");
		RequestManager_TestAPIOverlay->OnListFleetsResponseReceivedDelegate.RemoveDynamic(this, &ThisClass::OnListFleetsResponseReceived); 
	}
	
	//when succeed in getting "FleetIds" let add the same number of WBP_FleetId for found FleetIds on to the ::WBP_FleetBox::ScrollBox:
	if (bWasSuccessful)
	{
		for (FString FleetId : DSListFleetsReponse.FleetIds)
		{
			//step1: CreateWidget<WBP_child>
			//SHOCKING NEWs: this is the first time we pass in "UWidget" for CreatWidget (that 'OwningObject' could only accept 'PC/UWorld/UGameInstance' or 'UWidget/UWidgetTree' ). Because WBP_Overlay::WBP_Box::FleetId hence it works! I assume we can also pass in WBP_X (ScrollBox_ListFleets) as well? yeah! I believe so!
			UUW_FleetId* UW_FleetId = CreateWidget<UUW_FleetId>(this, UW_FleetId_Class);

			//step2: set InChild::Properties
			UW_FleetId->TextBlock_FleetId->SetText(FText::FromString(FleetId));
			
			//step3: Parent->ClearChild() - if needed (you can move this line out for both cases lol)
			WBP_ListFleetsBox->ScrollBox_ListFleets->ClearChildren();
			
			//step4: Parent->AddChild(InChild) 
			WBP_ListFleetsBox->ScrollBox_ListFleets->AddChild(UW_FleetId);
		}
	}
	//when not succeed, let add a single WBP_FleetId with WBP_FleetId::TextBlock="something went wrong"
	else
	{
		UUW_FleetId* UW_FleetId = CreateWidget<UUW_FleetId>(this, UW_FleetId_Class);
		//you can directly pass in DSResponse::errorMessage/errorType, but we're showing to user, so just make it simple lol (you already have the LOG via Dump already)
		UW_FleetId->TextBlock_FleetId->SetText(FText::FromString(TEXT("Something went wrong")));
		WBP_ListFleetsBox->ScrollBox_ListFleets->ClearChildren();
		WBP_ListFleetsBox->ScrollBox_ListFleets->AddChild(UW_FleetId);
	}

	//EITHER of the cases, let's enable the button back!
	WBP_ListFleetsBox->Button_ListFleets->SetIsEnabled(true);
}