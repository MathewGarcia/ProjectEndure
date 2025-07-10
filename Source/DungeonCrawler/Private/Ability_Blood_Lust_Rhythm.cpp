// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Blood_Lust_Rhythm.h"

#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

void UAbility_Blood_Lust_Rhythm::execute_Implementation()
{
	Super::execute_Implementation();


	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			player->SetPlayerState(PlayerStates::NONE);
			TWeakObjectPtr<APlayerCharacter> safePlayer = player;
			FBloodLustRhythmDelegate = player->OnEnemyDamageTaken.AddLambda([this, safePlayer](AEnemy* EnemyHit,float& DamageAmount)
				{
					if (!EnemyHit || !safePlayer.IsValid()) return;
					if (APlayerCharacter* localPlayer = Cast<APlayerCharacter>(safePlayer.Get())) {
						float PredictedHealth = EnemyHit->Health - DamageAmount;

						if (PredictedHealth <= 0.f)
						{
							++CurrentAmtKills;
						
							if(CurrentAmtKills >= KillsNeeded)
							{
								PlayWidget(localPlayer);
								PlayNiagaraSystem(localPlayer);
								localPlayer->MontageSpeed = FMath::Max(1.f,localPlayer->MontageSpeed * AttackSpeedModifierPercent);
								PlaySound();
								GetWorld()->GetTimerManager().SetTimer(FDurationTimer, [this,safePlayer]()
								{
										if (APlayerCharacter* localPlayer = Cast<APlayerCharacter>(safePlayer.Get())) {
											localPlayer->MontageSpeed = 1.f;
											CurrentAmtKills = 0;
										}
								}, Duration, false);
							}
					
							
						}
					}

				});
		}
	}
}

bool UAbility_Blood_Lust_Rhythm::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this);
		}
	}
	return false;
}

void UAbility_Blood_Lust_Rhythm::PlaySound()
{
	if (SoundToPlay) {
		UGameplayStatics::PlaySound2D(GetWorld(), SoundToPlay);
	}
}

void UAbility_Blood_Lust_Rhythm::PlayWidget(APlayerCharacter* Player)
{
	if (!Player) return;

	if(Player->CurrentLivesWidget)
	{
		Player->CurrentLivesWidget->SetWidgetClass(WidgetToShow);
		Player->CurrentLivesWidget->SetVisibility(true);
		FTimerHandle LocalTimer;
		GetWorld()->GetTimerManager().SetTimer(LocalTimer, [Player]
		{
				Player->ResetCurrentLivesWidget();
				Player->CurrentLivesWidget->SetVisibility(false);

		}, 1.f, false);
	}

}

void UAbility_Blood_Lust_Rhythm::PlayNiagaraSystem(APlayerCharacter* player)
{
	if (!player) return;

	if (NiagaraSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, player->GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true);
	}
}
