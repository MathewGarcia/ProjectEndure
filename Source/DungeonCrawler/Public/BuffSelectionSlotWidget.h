// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffDataAsset.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "BuffSelectionSlotWidget.generated.h"




class UBuffSelectionWidget;
class UBuffDataAsset;
class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UBuffSelectionSlotWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	UTextBlock* BuffName;

	UPROPERTY(meta=(BindWidget))
	UImage* DisplayingImage;

	UPROPERTY()
	UBuffDataAsset* BuffData;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void NativeConstruct() override;


	FTimerHandle FRollingTimer;

	FTimerHandle FStopTimer;

	void ResetData();

	TWeakObjectPtr<UBuffDataAsset> SafeDataAsset;

	
	UPROPERTY()
	TWeakObjectPtr<UBuffDataAsset> ChoseDataAssetSafe;

	UPROPERTY()
	TArray<UBuffDataAsset*>AllBuffs;

	UPROPERTY()
	UBuffSelectionWidget* OwningWidget;

	void RollDrop();

};
