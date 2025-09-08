// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/LocalPlayerSubsystem_DS.h"
#include "UW_Expand.generated.h"

class UTextBlock;
class UButton;
class URequestManager_Portal;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_Expand : public UUserWidget
{
	GENERATED_BODY()

public:
     UPROPERTY(EditAnywhere)
	TSubclassOf<URequestManager_Portal> RequestManager_Portal_Class; //select class for it
	UPROPERTY()
	TObjectPtr<URequestManager_Portal> RequestManager_Portal;
	
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_SignOut;	//got work to do

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ButtonText;
	
	UPROPERTY(EditAnywhere)
	FSlateColor Color_Hover;
	UPROPERTY(EditAnywhere)
	FSlateColor Color_Unhover;
protected:
	virtual void NativeConstruct() override;   //like BeginPLay in UActorComponent and AActor::BeginPlaye
	virtual void NativePreConstruct() override; //like construction script in normal BP_Something
	
	UFUNCTION()
	void OnButtonClicked();
	UFUNCTION()
	void OnButtonHovered();
	UFUNCTION()
	void OnButtonUnhovered();
	
	ULocalPlayerSubsystem_DS* GetLocalPlayerSubsystem_DS() const;
};
