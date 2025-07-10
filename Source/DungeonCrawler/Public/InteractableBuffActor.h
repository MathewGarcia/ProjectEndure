// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "InteractableBuffActor.generated.h"

class UBuffSelectionWidget;
class UBuffDataAsset;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API AInteractableBuffActor : public AInteractableActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category = "Buffs map")
	TMap<UBuffDataAsset*,float> BuffsMap;


	UPROPERTY(EditAnywhere, Category = "Max buffs")
	int MaxBuffsDrop = 3;


	UPROPERTY(EditAnywhere, Category = "WidgetClass")
	TSubclassOf<UBuffSelectionWidget> BuffSelectionWidgetClass;

	

	UBuffDataAsset* ChooseBuff();

	UFUNCTION()
	void OnBuffSelected();

protected:
	virtual void Interact_Implementation() override;

private:

	TArray<UBuffDataAsset*> ChosenBuffs;

	TArray<UBuffDataAsset*> AllBuffs;

	UPROPERTY()
	UBuffSelectionWidget* BuffSelectionWidget;


	
	
};
