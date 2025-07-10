// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "SaveStoneActor.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ASaveStoneActor : public AInteractableActor 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category = "Niagra To Play")
	UNiagaraSystem* NiagaraSystem;

	virtual void Interact_Implementation() override;	

};
