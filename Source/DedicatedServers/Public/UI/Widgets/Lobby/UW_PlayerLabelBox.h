// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoAndState/LobbyInfo.h"
#include "Types/LobbyPlayerInfo.h"
#include "UW_PlayerLabelBox.generated.h"

class UUW_PlayerLabel;
class UScrollBox;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_PlayerLabelBox : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	
	void UpdateHUD_PlayerInfos(ALobbyInfo* LobbyInfo);
	
	UFUNCTION()
	void OnLobbyInfoInitCallback(ALobbyInfo* LobbyInfo);
	
	UFUNCTION()
	void OnPlayerInfoAddedCallback(const FLobbyPlayerInfo& PlayerInfo);
	UFUNCTION()
	void OnPlayerInfoRemovedCallback(const FLobbyPlayerInfo& PlayerInfo);

	//helpers:
	UUW_PlayerLabel* GetPlayerLabelByPlayerName(const FString& PlayerName);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_UserNames;	

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUW_PlayerLabel> UW_PlayerLabel_Class;
protected:
	
};
