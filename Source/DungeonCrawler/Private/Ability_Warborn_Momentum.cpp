// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Warborn_Momentum.h"

#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void UAbility_Warborn_Momentum::execute_Implementation()
{
	Super::execute_Implementation();

	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			player->SetPlayerState(PlayerStates::NONE);
			TWeakObjectPtr<APlayerCharacterState> safePlayerState = PCS;
			FOnDamageDelegateHandle = player->OnEnemyDamageTaken.AddLambda([this,safePlayerState](AEnemy*EnemyHit,float& Damage)
			{
					if (!EnemyHit || !safePlayerState.IsValid()) return;
					if (APlayerCharacterState* localPCS = Cast<APlayerCharacterState>(safePlayerState.Get())) {
						UE_LOG(LogTemp,Warning,TEXT("Enemy HIT LAMBDA"))
						if (EnemyHit->Health <= 0.f)
						{
							localPCS->playerStats.currentStamina += localPCS->GetTotalStamina() * AmtPercent;
							localPCS->playerStats.currentStamina = FMath::Clamp(localPCS->playerStats.currentStamina, 0.f, localPCS->GetTotalStamina());
							if (APlayerCharacter*LocalPlayerCharacter= Cast<APlayerCharacter>(localPCS->GetPawn()))
							{
								if (APlayerController* PC = Cast<APlayerController>(LocalPlayerCharacter->GetController())) {
									if (AInGamePlayerHUD* playerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
									{
										if (playerHUD->GetMainUIWidget())
										{
											playerHUD->GetMainUIWidget()->UpdateProgressBar("Stamina", localPCS->playerStats.currentStamina / localPCS->GetTotalStamina());

										}
									}
								}
							}
						}
					}
					
			});
		}
	}

}

bool UAbility_Warborn_Momentum::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this);
		}
	}
	return false;
}

void UAbility_Warborn_Momentum::Logic()
{
	Super::Logic();
}
