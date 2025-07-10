// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragWidget.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
UCLASS()
class DUNGEONCRAWLER_API UDragWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UImage* image;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AbilityName;

};
