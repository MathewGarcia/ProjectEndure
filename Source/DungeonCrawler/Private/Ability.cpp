// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void UAbility::execute_Implementation() {

}

bool UAbility::bShouldExecute_Implementation()
{
	return true;
}

void UAbility::Logic()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			player->SetPlayerState(PlayerStates::ABILITY);
			if (OnAbilityUsed.IsBound())
			{
				OnAbilityUsed.Broadcast();
			}
		}
	}
}

void UAbility::SetIsLearned(bool bNewIsLearned) {
	bIsLearned = bNewIsLearned;
}

void UAbility::SerializeObject(FAbilitySaveData& OutData)
{

	OutData.AbilityClass = GetClass();
	FVariableVariant VariableVariant = FVariableVariant();
	VariableVariant.AbilityTalentRow = AbilityTalentRow;
	OutData.AbilityMetaData.Add("AbilityRow",VariableVariant);

}

void UAbility::DeserializeObject(const FAbilitySaveData& InData)
{
	if(const auto& RowData = InData.AbilityMetaData.Find("AbilityRow"))
	{
		if (RowData)
		{
			AbilityTalentRow = RowData->AbilityTalentRow;
		}
	}
}

void UAbility::OnTriggered()
{
}
