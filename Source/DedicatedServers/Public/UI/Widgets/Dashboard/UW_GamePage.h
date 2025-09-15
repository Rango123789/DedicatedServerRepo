// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_GamePage.generated.h"

class URequestManager_GameSessions;
class UUW_JoinGame;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_GamePage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UUW_JoinGame> WBP_JoinGame;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URequestManager_GameSessions> RequestManager_GameSessions_Class;
	UPROPERTY()
	TObjectPtr<URequestManager_GameSessions> RequestManager_GameSessions;

	UFUNCTION()
	void OnStatusMessageDelegateBroadcast(const FString& StatusMessage, bool bResetWidgetButtons);
	UFUNCTION()
	void OnJoinGameButtonClicked();
protected:
	virtual void NativeConstruct() override;
};
