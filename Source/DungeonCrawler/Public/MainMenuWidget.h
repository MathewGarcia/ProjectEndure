// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;


	UPROPERTY(meta = (BindWidget))
	UButton* NewPlayerButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;


	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnNewPlayerButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();
};
