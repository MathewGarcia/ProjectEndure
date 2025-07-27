// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Blood_Pact.h"

#include "Enemy.h"
#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetSystemLibrary.h"

void UAbility_Blood_Pact::execute_Implementation()
{
	Super::execute_Implementation();
	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (MontageToPlay)
				player->PlayAnimMontage(MontageToPlay, 1.f);
			if (NiagaraSystemToPlay && player->GetMesh()) {
				if(UNiagaraComponent*NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystemToPlay,player->GetMesh(),"spine_01",FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::Type::SnapToTargetIncludingScale,false))
				{
					NC = NiagaraComponent;
				}
			}
		}
	}

}

bool UAbility_Blood_Pact::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			UWorld* World = GetWorld();
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && World && !World->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost) && !bActivated;
		}
	}
	return false;
}

void UAbility_Blood_Pact::Logic()
{
	Super::Logic();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			UWorld* World = player->GetWorld();
			if (!World) return;
			FVector Center = player->GetActorLocation();
			float Radius = 400.f;
			TArray<AActor*>actorsToIgnore;
			actorsToIgnore.Add(player);
			TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
			TArray<AActor*>HitActors;
			if (UKismetSystemLibrary::SphereOverlapActors(World, Center, Radius, ObjectTypes, AEnemy::StaticClass(), actorsToIgnore, HitActors)) {
				float ClosestDistance = 999999.f;
				for(AActor* HitActor : HitActors)
				{
					if(AEnemy*HitEnemy = Cast<AEnemy>(HitActor))
					{
						float currentDistance = (HitEnemy->GetActorLocation() - player->GetActorLocation()).Size();
						if(currentDistance < ClosestDistance)
						{
							ClosestDistance = currentDistance;
							LinkedEnemy = HitEnemy;
						}
					}
				}
			}
			if(LinkedEnemy)
			{
				bActivated = true;
				player->OnPlayerDamaged.Remove(FBloodpactDelegate);
				TWeakObjectPtr<APlayerCharacterState> safePCS = PCS;
				TWeakObjectPtr<AEnemy> SafeLinkedEnemy = LinkedEnemy;
				FBloodpactDelegate = player->OnPlayerDamaged.AddLambda([this, safePCS,SafeLinkedEnemy](float& Damage, const FDamageEvent& DamageEvent, AController* Controller, AActor* DamageCauser)
					{
						if (!safePCS.IsValid() || !SafeLinkedEnemy.IsValid()) return;
						if(APlayerCharacterState*localPCS = Cast<APlayerCharacterState>(safePCS.Get()))
						{
							if (APlayerCharacter* player = Cast<APlayerCharacter>(localPCS->GetPawn())) {
								if (AEnemy* localLinkedEnemy = Cast<AEnemy>(SafeLinkedEnemy.Get()))
								{
									float TakenDamage = localLinkedEnemy->CalculateDamage();
									DamageRecieved += TakenDamage;
									localLinkedEnemy->TakeDamage(TakenDamage, FDamageEvent(), player->GetController(), player);
									if(localLinkedEnemy->Health <= 0.f)
									{
										player->OnPlayerDamaged.Remove(FBloodpactDelegate);
										localPCS->playerStats.currentHealth += DamageRecieved;
										if(APlayerController*PC = Cast<APlayerController>(player->GetController()))
										{
											if(AInGamePlayerHUD*playerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
											{
												if(playerHUD->GetMainUIWidget())
												{
													playerHUD->GetMainUIWidget()->UpdateProgressBar("Health", localPCS->playerStats.currentHealth / localPCS->GetTotalHealth());
												}
											}
										}
									}
								}
							}
						}
					});
				// Ensure any previous timers are cleared before setting new ones
				if (World->GetTimerManager().IsTimerActive(FCooldown)) {
					World->GetTimerManager().ClearTimer(FCooldown);
				}
				World->GetTimerManager().SetTimer(FCooldown, [this,safePCS]()
				{
						if (!safePCS.IsValid() || !NC.IsValid() ) return;
						if (APlayerCharacterState* localPCS = Cast<APlayerCharacterState>(safePCS.Get()))
						{
							if (APlayerCharacter* player = Cast<APlayerCharacter>(localPCS->GetPawn())) {
								player->OnPlayerDamaged.Remove(FBloodpactDelegate);
							}
							bActivated = false;
						}
						if(UNiagaraComponent*NiagaraComponent = NC.Get())
						{
							NiagaraComponent->DestroyComponent();
						}
				}, Duration,false);
				World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
			}
		}
	}

}
