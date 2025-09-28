// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Lobby/UW_PlayerLabelBox.h"
#include "UI/Widgets/Lobby/UW_PlayerLabel.h"
#include "Components/ScrollBox.h"
#include "Game/DSGameState.h"
#include "InfoAndState/LobbyInfo.h"

//this is called at the time you do "CreateWidget<UUW_PlayerLabelBox>" (even before AddToViewport or OtherParent.AddChild(WidgetObject))
//this is where we want bind this::helpers to AGameState::ALobbyInfo::2 delegates - but at this time AGameState::ALobbyInfo (constructed in AGameState::BeginPlay, not in AGameState::constructor) may or may not valid yet. hence we want to bind to AGameState::OnLobbyInfoInit to have a second chance (I see this practice in GAS course about NiagaraComponent), that's the whole reason stephen use "ReplicatedUsing" to have OnRep_LobbyInfo to broadcast that delegate!
void UUW_PlayerLabelBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();
//access AGameState::ALobbyInfo::2 delegates and bind this::callbacks to it, consider AGameState::ALobbyInfo isn't valid at the time and so do the same thing in AGameState::OnLobbyInfoInitDelegate.AddLambda([](){do the same thing})
	ADSGameState* DSGameState = GetWorld()->GetGameState<ADSGameState>();
	if (DSGameState == nullptr) return; //99% it is valid before the construction of this widget, but we're not sure its ::LobbyInfo (which is actor-like) that is constructed in AGameState::constructor is valid yet

	ALobbyInfo* LobbyInfo = DSGameState->LobbyInfo;
	if (IsValid(LobbyInfo))
	{
		//more convenient option, so that we can do other thing as well, but some checking could be "double" lol
		OnLobbyInfoInitCallback(LobbyInfo); 
	}
	else
	{
		//we can't 'add lambda' for dynamic multicast delegate? strange lol
		DSGameState->OnLobbyInfoInit.AddDynamic(this, &ThisClass::OnLobbyInfoInitCallback);
	}
}

void UUW_PlayerLabelBox::OnLobbyInfoInitCallback(ALobbyInfo* LobbyInfo)
{
	if(IsValid(LobbyInfo) == false) return; //I don't think it is nullptr unless you forget to set bReplicates=true++ for it
	
	LobbyInfo->OnPlayerInfoAdded.AddDynamic(this, &ThisClass::OnPlayerInfoAddedCallback);
	LobbyInfo->OnPlayerInfoRemoved.AddDynamic(this, &ThisClass::OnPlayerInfoRemovedCallback);

	UpdateHUD_PlayerInfos(LobbyInfo);
}

void UUW_PlayerLabelBox::UpdateHUD_PlayerInfos(ALobbyInfo* LobbyInfo)
{
	//must clear "ONCE" only outside the loop, not inside:
	ScrollBox_UserNames->ClearChildren();
	
	//the reason why stephen move the arry to protected+ and then create a getter is that we don't want to directly modify the array without doing though the ::AddPlayerInfo and ::RemovePlayerInfo that also handle teachnical things about "Fast array serializer" as well!
	for (auto& PlayerInfo : LobbyInfo->GetPlayerInfoArray().Items) //LobbyInfo::LobbyPlayerInfoArray::Items
	{
		//UPDATE: let's make sure it isn't in the box before adding it!
		if (GetPlayerLabelByPlayerName(PlayerInfo.Username)) return;
	
		//because this block of code is identical to "OnPlayerInfoRemovedCallback", so you can basically call it instead lol :D :D, but I don't bother to!
		UUW_PlayerLabel* WBP_PlayerLabel = CreateWidget<UUW_PlayerLabel>(this, UW_PlayerLabel_Class);

		WBP_PlayerLabel->SetPlayerName(PlayerInfo.Username);
		
		ScrollBox_UserNames->AddChild(WBP_PlayerLabel);
	}
}


//TODO1: DYNAMICALLY spawn and then add UW_Child on ::ScrollBox_parent is more easy
void UUW_PlayerLabelBox::OnPlayerInfoAddedCallback(const FLobbyPlayerInfo& PlayerInfo)
{
	//UPDATE: let's make sure it isn't in the box before adding it!
	if (GetPlayerLabelByPlayerName(PlayerInfo.Username)) return;
	
	//4 steps:
	//DO NOT confuse: the owning object of a widget () VS the parent widget of a widget (where exactly it attached to and so on)
	//it is exactly like "UActorComponent" always hast its Owner/Outer = HostingActor, but its attached parent could be either RootComponent or other UActorComponent!

	UUW_PlayerLabel* WBP_PlayerLabel = CreateWidget<UUW_PlayerLabel>(this, UW_PlayerLabel_Class);

	WBP_PlayerLabel->SetPlayerName(PlayerInfo.Username);
		
	ScrollBox_UserNames->AddChild(WBP_PlayerLabel);
}

//TODO2: update the this::UScrollBox accordlingly, remove the UW_PlayerLabel::TextBlock_UserName that match PlayerInfo.UserName, need we store an array/map of them?
//you can loop through each ScrollBox::children, cast to UW_PlayerLabel[::TextBlock_Username] and then check their text content again PlayerInfo.Username I guess?
void UUW_PlayerLabelBox::OnPlayerInfoRemovedCallback(const FLobbyPlayerInfo& PlayerInfo)
{

	TArray<UWidget*> Widgets = ScrollBox_UserNames->GetAllChildren();
	for (auto& Widget : Widgets)
	{
		UUW_PlayerLabel* UW_PlayerLabel = Cast<UUW_PlayerLabel>(Widget);
		if (UW_PlayerLabel && UW_PlayerLabel->GetPlayerName() == PlayerInfo.Username)
		{
			ScrollBox_UserNames->RemoveChild(UW_PlayerLabel);
			return;
		}
	}
}

//this one can be re-used! hence you can make the "OnPlayerInfoRemovedCallback above!
UUW_PlayerLabel* UUW_PlayerLabelBox::GetPlayerLabelByPlayerName(const FString& PlayerName)
{
	TArray<UWidget*> Widgets = ScrollBox_UserNames->GetAllChildren();
	for (auto& Widget : Widgets)
	{
		UUW_PlayerLabel* UW_PlayerLabel = Cast<UUW_PlayerLabel>(Widget);
		if (UW_PlayerLabel && UW_PlayerLabel->GetPlayerName() == PlayerName)
		{
			return UW_PlayerLabel;
		}
	}

	return nullptr;
}