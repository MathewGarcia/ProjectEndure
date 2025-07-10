// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"
/**
 * 
 */

UINTERFACE(MinimalAPI,Blueprintable)
class UInteractableInterface : public UInterface
{
public:

	GENERATED_BODY()

};


class DUNGEONCRAWLER_API IInteractableInterface {

	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	 void Interact();
};