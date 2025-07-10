// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Infinite_Ablities.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"

void UAbility_Infinite_Ablities::execute_Implementation()
{
	Super::execute_Implementation();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			TWeakObjectPtr<UAbility_Infinite_Ablities> safeAbility = this;

			player->OnEnemyDamageTaken.Remove(FInfiniteAbilitiesDelegateHandle);

			FInfiniteAbilitiesDelegateHandle =	player->OnEnemyDamageTaken.AddLambda([safeAbility](AEnemy*Enemy, float&Damage)
				{
					if (!safeAbility.IsValid()) return;

					if (UAbility_Infinite_Ablities* localAbility = safeAbility.Get()) {
						float PredictedHP = Enemy->Health - Damage;

						if (PredictedHP <= 0.f) {
							++localAbility->CurrentKills;
							if (localAbility->CurrentKills >= localAbility->KillsNeeded) {
								if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(localAbility->GetOuter()))
								{
									TWeakObjectPtr<APlayerCharacterState>safePCS = PCS;
									PCS->playerStats.currentMana = 99999.f;
									PCS->GetWorldTimerManager().SetTimer(localAbility->FDuration, [safePCS, localAbility]
										{

											if (!safePCS.IsValid() || !localAbility) return;

											if (APlayerCharacterState* localPCS = safePCS.Get())
											{
												localPCS->playerStats.currentMana = localPCS->GetTotalMana();
												if (APlayerCharacter* localPlayer = Cast<APlayerCharacter>(localPCS->GetPawn()))
												{

													localPlayer->OnEnemyDamageTaken.Remove(localAbility->FInfiniteAbilitiesDelegateHandle);
												}
											}
										}, localAbility->Duration, false);

								}
							}
						}
					}

				});

			player->SetPlayerState(PlayerStates::NONE);

		}
	}
}

bool UAbility_Infinite_Ablities::bShouldExecute_Implementation()
{

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this);
		}
	}
	return false;
}


