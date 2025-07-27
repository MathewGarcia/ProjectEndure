// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Intelligence_Penalty.h"

#include "Enemy.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "VariableVariant.h"
#include "Kismet/GameplayStatics.h"

UAbility_Intelligence_Penalty::UAbility_Intelligence_Penalty(): Amount(0)
{
	AbilityActivationType = EAbilityActivationType::Event;
}

void UAbility_Intelligence_Penalty::execute_Implementation()
{
	Super::execute_Implementation();
	UWorld* World = GetWorld();
	if (!World) return;
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(World, AEnemy::StaticClass(), Enemies);
	for (AActor* EnemyActor : Enemies)
	{
		if (AEnemy* Enemy = Cast<AEnemy>(EnemyActor))
		{
			for (int i = 0; i < Amount; ++i) {
				// Defensive: ensure IntelligencePenalty is not zero or negative before applying penalty
				Enemy->IntelligencePenalty -= Enemy->IntelligencePenalty * 0.03;
			}
		}
	}
}

bool UAbility_Intelligence_Penalty::bShouldExecute_Implementation()
{
	if(APlayerCharacter*player = Cast<APlayerCharacter>(GetOuter()))
	{
		if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
		{
			return PCS->LearnedAbilities.Contains(this);
		}
	}
	return false;
}

void UAbility_Intelligence_Penalty::SerializeObject(FAbilitySaveData& OutData)
{
	Super::SerializeObject(OutData);
	FVariableVariant VariableVariant = FVariableVariant();
	VariableVariant.IntVar = Amount;
	OutData.AbilityMetaData.Add("Amount", VariableVariant);
}

void UAbility_Intelligence_Penalty::DeserializeObject(const FAbilitySaveData& InData)
{
	Super::DeserializeObject(InData);
	if (InData.AbilityMetaData.Contains("Amount")) {
		Amount = InData.AbilityMetaData["Amount"].IntVar;
	}
}
