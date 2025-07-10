// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementDebuff.h"
#include "ShadowDebuff.generated.h"

class ADebuffActor;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UShadowDebuff : public UElementDebuff
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category = "Debuff")
	TSubclassOf<ADebuffActor> ActorToSpawn;

	virtual void ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser) override;

};
