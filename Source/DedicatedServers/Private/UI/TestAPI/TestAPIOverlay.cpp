// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TestAPI/TestAPIOverlay.h"

#include "Components/Button.h"
#include "UI/API/UW_ListFleetsBox.h"
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
	WBP_ListFleetsBox->Button_ListFleets->OnClicked.AddDynamic(RequestManager_TestAPIOverlay, &URequestManager_TestAPIOverlay::OnClicked_ListFleetsButton);
}
