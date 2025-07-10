// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LivesWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ULivesWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* LivesText;

	void UpdateLivesText(int newVal);
	
};
