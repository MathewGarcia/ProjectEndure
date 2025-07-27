// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityBlink.h"

#include "Enemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Kismet/GameplayStatics.h"

void UAbilityBlink::execute_Implementation()
{
	Super::execute_Implementation();
	if (APlayerCharacterState * PCS = Cast<APlayerCharacterState>(GetOuter())) {
			if(APlayerCharacter*player = Cast<APlayerCharacter>(PCS->GetPawn()))
			{
				FVector StartLocation = player->GetActorLocation();
				FVector EndLocation = StartLocation + (player->GetActorForwardVector() * 800.f);
				player->SetActorLocation(EndLocation, true);
				player->SetPlayerState(PlayerStates::NONE);
				PlaySound(player);
				PlayNiagaraSystem(player);
				if (player->GetWorld()) {
					if (player->GetWorld()->GetTimerManager().IsTimerActive(FCooldown)) {
						player->GetWorld()->GetTimerManager().ClearTimer(FCooldown);
					}
					player->GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
				}
			}
	}
	else if(AEnemy*enemy = Cast<AEnemy>(GetOuter()))
	{
			FVector StartLocation = enemy->GetActorLocation();
			FVector EndLocation = StartLocation + enemy->GetActorForwardVector() * 1000;
			PlaySound(enemy);
			PlayNiagaraSystem(enemy);
			enemy->SetActorLocation(EndLocation,true);
		
	}
}

bool UAbilityBlink::bShouldExecute_Implementation()
{
	if(AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
	{
		if (Enemy->EnemyParameters.DefensiveAbilities.Contains(this) && Enemy->GetStamina() >= staminaCost)
		{
			return true;
		}

	}
	else if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter())) {
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			UWorld* World = GetWorld();
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && World && !World->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Mana, manaCost);
		}
	}
	return false;
}

void UAbilityBlink::PlaySound(AActor* Actor)
{

	if (!Actor || !SoundToPlay) return;
	UWorld* World = Actor->GetWorld();
	if (!World) return;
	UGameplayStatics::PlaySoundAtLocation(World, SoundToPlay, Actor->GetActorLocation(), Actor->GetActorRotation());
}

void UAbilityBlink::PlayNiagaraSystem(AActor* Actor)
{
	if (!Actor || !NiagaraSystem) return;
	UWorld* World = Actor->GetWorld();
	if (!World) return;
	if(UNiagaraComponent*NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World,NiagaraSystem,Actor->GetActorLocation(),Actor->GetActorRotation()))
	{
		
	}
}
