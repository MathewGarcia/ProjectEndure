// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Arcane_Conduction.h"

#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Kismet/GameplayStatics.h"

void UAbility_Arcane_Conduction::execute_Implementation()
{
	Super::execute_Implementation();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			player->OnEnemyDamageTaken.Remove(FArcaneConduactionDelegateHandle);
			TWeakObjectPtr<APlayerCharacterState>safePCS = PCS;
			FArcaneConduactionDelegateHandle = player->OnEnemyDamageTaken.AddLambda([this,safePCS](AEnemy* Enemy, float& Damage)
				{
					if (!safePCS.IsValid()) return;
					APlayerCharacterState* localPCS = safePCS.Get();
					if (!localPCS) return;
					float ReturnMana = localPCS->GetTotalMana() * Percent;
					localPCS->playerStats.currentMana += ReturnMana;
					localPCS->playerStats.currentMana = FMath::Clamp(localPCS->playerStats.currentMana, 0.f, localPCS->GetTotalMana());

					UWorld* World = GetWorld();
					if(!World) return;
					if(APlayerController*PC = UGameplayStatics::GetPlayerController(World,0))
					{
						if(AInGamePlayerHUD*PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
						{
							if(UMainPlayerWidget*MainPlayerWidget = PlayerHUD->GetMainUIWidget())
							{
								MainPlayerWidget->UpdateProgressBar("Mana", localPCS->playerStats.currentMana / localPCS->GetTotalMana());
							}
						}
					}

				});
		}
	}

}

bool UAbility_Arcane_Conduction::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this);
		}
	}
	return false;
}
