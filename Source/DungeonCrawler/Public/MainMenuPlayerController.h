// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UMainMenuWidget;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Main Menu Widget")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

protected:
	virtual void BeginPlay() override;

};
