// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Iron_Skin.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Iron_Skin : public UAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Duration")
	float Duration;

	UPROPERTY(EditAnywhere, Category = "Material To Change To")
	UMaterialInstance* IronSkinMaterial;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;
	
	virtual void Logic() override;
protected:
	FTimerHandle FCooldown;

};
