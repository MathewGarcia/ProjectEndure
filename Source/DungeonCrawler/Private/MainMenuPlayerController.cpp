// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"

#include "MainMenuWidget.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	if(MainMenuWidgetClass)
	{
		if(UMainMenuWidget*MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass))
		{
			MainMenuWidget->AddToViewport();


			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
			SetInputMode(InputMode);
		}
	}


}
