// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerHUD.h"

#include "PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "MainPlayerWidget.h"
#include "Widget_Pause_Menu.h"

AInGamePlayerHUD::AInGamePlayerHUD()
{
}

void AInGamePlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if(MainPlayerWidgetClass)
	{
		MainUIWidget = CreateWidget<UMainPlayerWidget>(GetWorld(), MainPlayerWidgetClass);
		if(MainUIWidget)
		{
			MainUIWidget->AddToViewport();
		}
	}

	if(PauseMenuWidgetClass)
	{
		PauseWidget = CreateWidget<UWidget_Pause_Menu>(GetWorld(), PauseMenuWidgetClass);
		if(PauseWidget)
		{
			PauseWidget->AddToViewport();
			PauseWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
