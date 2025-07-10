// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "AIController.h"
#include "Ability_Salamander_Dig.generated.h"

class UNiagaraSystem;
class AAIController;
struct FAIRequestID;
struct FPathFollowingResult;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Salamander_Dig : public UAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> UndergroundCameraShake;

	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> BurstingCameraShake;

	UPROPERTY(EditAnywhere,Category = "Sound To Play")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere,Category = "Niagra System")
	UNiagaraSystem* NiagaraSystemToPlay;

	UPROPERTY(EditAnywhere,Category = "Time")
	float Time = 2.f;
	
	virtual void execute_Implementation() override;

	virtual void Logic() override;

	virtual bool bShouldExecute_Implementation() override;

	UFUNCTION()
	void JumpOut();

	bool bIsActive = false;

	FTimerHandle FCooldown;

	FDelegateHandle JumpoutDelegateHandle;

	FTimerHandle FUndergroundShake;

};
