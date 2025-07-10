// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractableTextWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UInteractableTextWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta=(BindWidget))
	UTextBlock* InteractTextBlock;

	void UpdateInteractTextBlock(const FString& Text);
	
};
