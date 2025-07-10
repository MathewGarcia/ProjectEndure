// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TileView.h"
#include "Widget_Quest.generated.h"

class ANPC;
class UQuestDataAsset;
/**
 * 
 */
class UTextBlock;
class UButton;

UCLASS()
class DUNGEONCRAWLER_API UWidget_Quest : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* QuestText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DescriptionText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RewardsText;

	UPROPERTY(meta = (BindWidget))
	UButton* AcceptButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DenyButton;

	UPROPERTY(meta = (BindWidget))
	UTileView* PossibleRewards;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* EXPTextBlock;



	virtual void NativeConstruct() override;


	UFUNCTION()
	void OnAcceptClicked();

	UFUNCTION()
	void OnDenyClicked();

public:
	UPROPERTY()
	UQuestDataAsset* QuestDataAsset;

	UPROPERTY()
	ANPC* OwningNPC;

	void InitInfo();
};
