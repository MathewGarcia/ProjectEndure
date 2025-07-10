// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Mirror_Self.generated.h"

enum class EGearType : uint8;
class AItem;
class APlayerCharacterState;
class AAIPlayerClones;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Mirror_Self : public UAbility
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Duration")
	float Duration;

	UPROPERTY(EditAnywhere, Category = "Clone Max")
	int CloneMax = 3;

	UPROPERTY(EditAnywhere, Category = "Clone Offset")
	float Offset = 300.f;

	UPROPERTY(EditAnywhere, Category = "ControllerToSpawn")
	TSubclassOf<AAIPlayerClones>ControllerToSpawn;

	UPROPERTY(EditAnywhere, Category = "PlayerToSpawn")
	TSubclassOf<APlayerCharacter> PlayerToSpawn;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

	void ClonePlayerStats(APlayerCharacter*ClonedPlayer, APlayerCharacterState*ClonedPlayerState, APlayerCharacterState* OGPlayerState);


private:
	FTimerHandle FCooldown;
	FTimerHandle FDuration;
	FTimerHandle FPlayerLiveCheck;

	TArray<APlayerCharacter*> PlayerClones;

	TArray<AAIPlayerClones*> AIPlayerClonesControllers;

	TArray<APlayerCharacterState*> PlayerCharacterStatesClones;

};
