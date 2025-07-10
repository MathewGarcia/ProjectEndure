// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputConfigData.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS()
class UInputConfigData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputAction* InputMove;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputAction* InputLook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAttack;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputAction* InputDodge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputUseItem;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAbility1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAbility2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAbility3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAbility4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAbility5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAbility6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAbilityHeal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputHandSwap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputInteractBag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputInteractStats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputInteractTalents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputInteractEquipment;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputPauseMenu;


};
