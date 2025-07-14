// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Bursting_Shield.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

void UAbility_Bursting_Shield::execute_Implementation()
{
	Super::execute_Implementation();


	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			player->PlayAnimMontage(MontageToPlay, 1.f);
			if(UNiagaraComponent*NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystemToPlay,player->GetMesh(),"spine_02",FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::Type::SnapToTargetIncludingScale,false,true))
			{
				NC = NiagaraComponent;
			}
		}
	}

}

bool UAbility_Bursting_Shield::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && !GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost) && !bActivated;
		}
	}
	return false;
}

void UAbility_Bursting_Shield::Logic()
{
	Super::Logic();

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			Shield = (PCS->GetTotalHealth()*0.3);
			float ReturnDamage = Shield;
			player->OnPlayerDamaged.Remove(FShieldDelegateHandle);
			TWeakObjectPtr<APlayerCharacter> playerSafe = player;
			bActivated = true;
			if (Shield > 0.f) {
				FShieldDelegateHandle =	player->OnPlayerDamaged.AddLambda([this,playerSafe,ReturnDamage](float& Damage, const FDamageEvent& DamageEvent, AController* Controller, AActor* DamageCauser)
					{
						if (!playerSafe.IsValid() || !NC.IsValid()) return;

						Shield -= Damage;
						UE_LOG(LogTemp, Warning, TEXT("LAMBDA FUNCTION CALLED %f"),Shield);

						Damage = 0;
						if (Shield <= 0.f)
						{
							if (APlayerCharacter* localPlayerPtr = playerSafe.Get()) {
								localPlayerPtr->OnPlayerDamaged.Remove(FShieldDelegateHandle);
								TArray<AActor*> actorsToIgnore;
								actorsToIgnore.Add(localPlayerPtr);
								if (UWorld* playerWorld = localPlayerPtr->GetWorld()) {
									UGameplayStatics::ApplyRadialDamage(playerWorld, ReturnDamage, localPlayerPtr->GetActorLocation(), 400.f, nullptr, actorsToIgnore, localPlayerPtr, localPlayerPtr->GetController(), true);
								}
								if (NC.IsValid()) {
									if (UNiagaraComponent* localNC = NC.Get())
									{
										if (!localNC->IsBeingDestroyed())
										{
											localNC->DestroyComponent();
										}
									}
									NC = nullptr;
								}
								bActivated = false;
							}
						}
					});
				GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
			}

		}

	}
}
