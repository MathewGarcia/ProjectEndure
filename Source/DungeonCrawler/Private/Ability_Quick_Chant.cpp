// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Quick_Chant.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void UAbility_Quick_Chant::execute_Implementation()
{
	Super::execute_Implementation();

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			CurrentKills = 0;
			player->OnEnemyDamageTaken.Remove(FQuickChantDelegateHandle);
			TWeakObjectPtr<APlayerCharacter> safePlayer = player;
			TWeakObjectPtr<UAbility_Quick_Chant> safeAbility = this;
			FQuickChantDelegateHandle = player->OnEnemyDamageTaken.AddLambda([safeAbility, safePlayer](AEnemy* Enemy, float& Damage)
				{
					if (!safePlayer.IsValid() || !safeAbility.IsValid()) return;
					UAbility_Quick_Chant* localAbility = safeAbility.Get();
					APlayerCharacter* localPlayer = safePlayer.Get();
					if (!localAbility || !localPlayer || !Enemy) return;

					float PredictedHealth = Enemy->Health - Damage;

					if (PredictedHealth <= 0.f) {

						if (UNiagaraComponent* NC = UNiagaraFunctionLibrary::SpawnSystemAttached(localAbility->NiagaraSystem, localPlayer->GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true))
						{
							localAbility->safeNC = NC;
						}

						++localAbility->CurrentKills;

						if (localAbility->CurrentKills >= localAbility->KillsNeeded) {

							localPlayer->MontageSpeed += localPlayer->MontageSpeed * localAbility->PercentIncrease;
							TWeakObjectPtr<AEnemy> SafeEnemy = Enemy;
								localPlayer->GetWorld()->GetTimerManager().SetTimer(localAbility->FDuration, [localPlayer,localAbility]
									{
										if (!localAbility || !localAbility->safeNC.IsValid()) return;
									if(UNiagaraComponent*localNC = localAbility->safeNC.Get())
									{
										localNC->DestroyComponent();
									}
										localPlayer->MontageSpeed = 1.f;
									
									}, localAbility->Duration, false);
							}
						
					}
					localPlayer->SetPlayerState(PlayerStates::NONE);
				});
		}
	}
}

bool UAbility_Quick_Chant::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this);
		}
	}
	return false;
}
