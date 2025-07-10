// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "BuffSelectionWidget.generated.h"

class UBuffSelectionSlotWidget;
class UBuffDataAsset;
class UTileView;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuffSelected);

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UBuffSelectionWidget : public UUserWidget , public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTileView* BuffSlotTV;


	TArray<UBuffDataAsset*> ChosenBuffs;

	void Init(const TArray<UBuffDataAsset*>& SelectedBuffs, const TArray<UBuffDataAsset*>& EveryBuff);


	TArray<UBuffDataAsset*> AllBuffs;

	FOnBuffSelected OnBuffSelected;

protected:
	virtual void NativeConstruct() override;
};
