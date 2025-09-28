// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DSGameModeBase.h"
#include "GameFramework/GameMode.h"
#include "DSLobbyGameMode.generated.h"

struct FProcessParameters;

/*DECLARE_LOG_CATEGORY_EXTERN(GameServerLog, Log, All);*/

/**
 * at the same time we know that, from the main module:
     BP_ShooterGameMode : AShooterGameMode :  AMatchGameMode : ADSGameModeBase : AGameMode
 * now we also have
      BP_LobbyGameMode :  [empty]             ADSLobbyGameMode : ADSGameModeBase : AGameMode
 */
UCLASS() //ADSGameMode has been renamed to 'ADSLobbyGameMode'
class DEDICATEDSERVERS_API ADSLobbyGameMode : public ADSGameModeBase 
{
	GENERATED_BODY()
public:
	ADSLobbyGameMode();
	//when this chain isn't triggered:
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = L"") override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	//this chain will trigger (they're manually exclusive):
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;

	
	virtual void OnCountDownTimerFinished(const ETimerType& InTimerType) override;

	void TryAcceptPlayterSession(const FString& PlayerSessionId, const FString& Username, FString& OutErrorMessage);
	//this is where you can reject or accept client to TECHNICALLY connect and open the server's level (not override this mean accept player by default):
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	void AddPlayerInfoToLobbyInfo(AController* InPC);
	void RemovePlayerInfoFromLobbyInfo(AController* InPC);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	ELobbyStatus LobbyStatus = ELobbyStatus::WaitingForPlayers;
	UPROPERTY(EditAnywhere)
	FCountdownTimerWrapper TimerWrapper_LobbyToGame;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> GameMap;

	UPROPERTY(EditAnywhere)
	int32 MinPlayers = 1.f; //for testing
	
	//for testing purpose
	UPROPERTY(EditAnywhere)
	bool bOverrideInitSeamlessTravelPlayer = true;

private:
	void InitGameLift();
	
	/*void InitGameLift_Practice();*/
	TSharedPtr<FProcessParameters> ProcessParams = nullptr;
};
