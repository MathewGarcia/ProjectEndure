// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "Drink_Interactable_Actor.generated.h"

class UNiagaraSystem;
class UAbility;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ADrink_Interactable_Actor : public AInteractableActor
{
	GENERATED_BODY()

	virtual void Interact_Implementation() override;

	UPROPERTY(EditAnywhere,Category = "Heal")
	TSubclassOf<UAbility> HealAbility;

	UPROPERTY(EditAnywhere,Category = "Niagara System To Play")
	UNiagaraSystem* NiagaraSystemToPlay;
};
