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
	UWorld* World = GetWorld();
	if (!World) return;
	if(MainPlayerWidgetClass)
	{
		MainUIWidget = CreateWidget<UMainPlayerWidget>(World, MainPlayerWidgetClass);
		if(MainUIWidget)
		{
			MainUIWidget->AddToViewport();
		}
	}
	if(PauseMenuWidgetClass)
	{
		PauseWidget = CreateWidget<UWidget_Pause_Menu>(World, PauseMenuWidgetClass);
		if(PauseWidget)
		{
			PauseWidget->AddToViewport();
			PauseWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
