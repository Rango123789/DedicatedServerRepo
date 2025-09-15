// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Dropdown.generated.h"

class ULocalPlayerSubsystem_DS;
class UTextBlock;
class UImage;
class UButton;
class UUW_Expand;
class UUW_Collapse;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UUW_Dropdown : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;
	
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UUW_Collapse> WBP_Collapse;
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UUW_Expand> WBP_Expand;
	
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> Button_Dropdown;	//got work to do
	
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UImage> Image_DropdownIcon;  //got work to do
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ButtonText;

	bool bExpand = false;
	
	UPROPERTY(EditAnywhere)
	FSlateColor Color_Hover;
	UPROPERTY(EditAnywhere)
	FSlateColor Color_Unhover;
	
	UPROPERTY(EditAnywhere)
	FSlateBrush Brush_Expand;
	UPROPERTY(EditAnywhere)
	FSlateBrush Brush_Collapse;

protected:
	virtual void NativeConstruct() override;   //like BeginPLay in UActorComponent and AActor::BeginPlaye
	virtual void NativePreConstruct() override; //like construction script in normal BP_Something
	
	UFUNCTION()
	void OnButtonClicked();
	UFUNCTION()
	void OnButtonHovered();
	UFUNCTION()
	void OnButtonUnhovered();
	
	void Expand();
	void Collapse();

	ULocalPlayerSubsystem_DS* GetLocalPlayerSubsystem_DS() const;
};
