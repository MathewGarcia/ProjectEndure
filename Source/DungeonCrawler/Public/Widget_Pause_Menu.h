// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Pause_Menu.generated.h"

/**
 * 
 */
class UButton;

UCLASS()
class DUNGEONCRAWLER_API UWidget_Pause_Menu : public UUserWidget
{
	GENERATED_BODY()


		UPROPERTY(meta=(BindWidget))
		UButton* ResumeButton;


		UPROPERTY(meta = (BindWidget))
		UButton* QuitButton;

		virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResumeButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();
	
};
