// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterController.h"

#include "InputTriggers.h"
#include "MainGameInstance.h"

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

	ResetCursor();
}

void APlayerCharacterController::ResetCursor()
{
	bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void APlayerCharacterController::ShowHideCursor(bool NewVal)
{
	bShowMouseCursor = NewVal;
	if(bShowMouseCursor)
	{
		SetInputMode(InputUI);
	}
	else
	{
		SetInputMode(InputGameOnly);
	}
}

void APlayerCharacterController::PlayCameraShake()
{
	PlayerCameraManager->StartCameraShake(CameraShake);
}
