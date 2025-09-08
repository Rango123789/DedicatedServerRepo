// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Dashboard/Dropdown/UW_Dropdown.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Widgets/Dashboard/Dropdown/UW_Collapse.h"
#include "UI/Widgets/Dashboard/Dropdown/UW_Expand.h"

void UUW_Dropdown::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Dropdown->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
	Button_Dropdown->OnHovered.AddDynamic(this, &ThisClass::OnButtonHovered);
	Button_Dropdown->OnUnhovered.AddDynamic(this, &ThisClass::OnButtonUnhovered);

	//NativeConstruct happen when the game start, NativePreConstruct happen even before the game start, just like you change one thing and you want to change other relevant things for that startup value set are compatible with each other. If you don't continue to change it in NativeConstruct, then they will be the final values right after the game just starts!
	//anyway, they're now redundant we did it all in preconstruct already!
	WidgetSwitcher->SetActiveWidget(WBP_Collapse);
	bExpand = false;
}

void UUW_Dropdown::NativePreConstruct()
{
	Super::NativePreConstruct();

	//you can in fact do it easily in WBP_::EventPreConstruct as well like we do MASSIVELY in GAS course lol.
	//you can also simply hardcode the default values in WBP_ as well lol, so there are absolutely "OPTIONAL" and for fun 
	//this will force you that whenever you change something, it will trigger these, which is could be very annoying lol :D :D
	//this is a good practice that we do it from C++ instead lol!
	FButtonStyle ButtonStyle;
	FSlateBrush ButtonBrush;
	ButtonBrush.TintColor = FSlateColor(FLinearColor(0,0,0,0)); //FColor(0,0,0,0) will also do
	ButtonStyle.Normal = ButtonBrush;
	ButtonStyle.Hovered = ButtonBrush;
	ButtonStyle.Pressed = ButtonBrush;
	ButtonStyle.Disabled = ButtonBrush;
	
	Button_Dropdown->SetStyle(ButtonStyle);

	Collapse();  //include bExpand=false
	OnButtonUnhovered();
}


void UUW_Dropdown::OnButtonClicked()
{
	if (bExpand == false)
	{
		Expand();
	}
	else
	{
		Collapse();
	}
}

void UUW_Dropdown::OnButtonHovered()
{
	TextBlock_ButtonText->SetColorAndOpacity(Color_Hover);
}

void UUW_Dropdown::OnButtonUnhovered()
{
	TextBlock_ButtonText->SetColorAndOpacity(Color_Unhover);
}

void UUW_Dropdown::Expand()
{
	bExpand = true;
	WidgetSwitcher->SetActiveWidget(WBP_Expand);
	Image_DropdownIcon->SetBrush(Brush_Expand);
}

void UUW_Dropdown::Collapse()
{
	bExpand = false;
	WidgetSwitcher->SetActiveWidget(WBP_Collapse);
	Image_DropdownIcon->SetBrush(Brush_Collapse);
}
