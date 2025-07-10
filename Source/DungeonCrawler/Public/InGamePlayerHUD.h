// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGamePlayerHUD.generated.h"

class UWidget_Pause_Menu;
class UBuffSelectionWidget;
class UTalentTreeWidget;
class ULevelUpWidget;
/**
 * 
 */
class UMainPlayerWidget;
UCLASS()
class DUNGEONCRAWLER_API AInGamePlayerHUD : public AHUD
{
	GENERATED_BODY()

public:

	AInGamePlayerHUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Widget Class")
	TSubclassOf<UMainPlayerWidget> MainPlayerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Class")
	TSubclassOf<UWidget_Pause_Menu> PauseMenuWidgetClass;

	UMainPlayerWidget* GetMainUIWidget()
	{
		return MainUIWidget;
	}

	UPROPERTY()
	ULevelUpWidget* LevelUpWidget;

	UPROPERTY()
	UTalentTreeWidget* TalentTreeWidget;

	UPROPERTY()
	UBuffSelectionWidget* BuffSelectionWidget;

	UPROPERTY()
	UWidget_Pause_Menu* PauseWidget;
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UMainPlayerWidget* MainUIWidget;
	
};
