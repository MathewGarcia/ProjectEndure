// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "StatItemWidget.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UStatItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()


public:
	UPROPERTY(meta=(BindWidget))
	UButton* DecreaseButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatLevelText;

	UPROPERTY(meta = (BindWidget))
	UButton* IncreaseButton;


	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	virtual void NativeConstruct() override;
	
};
