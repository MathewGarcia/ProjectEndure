// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityRainingArrows.h"
#include "PlayerCharacter.h"
#include "Enemy.h"
#include "MainGameInstance.h"

void UAbilityRainingArrows::execute_Implementation()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		if (Enemy->MiscMontages)
			Enemy->PlayAnimMontage(Enemy->MiscMontages, 1, "RainingArrows");
	}
	Super::execute_Implementation();
}

bool UAbilityRainingArrows::bShouldExecute_Implementation()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		UWorld* World = GetWorld();
		if (Enemy->EnemyParameters.OffensiveAbilities.Contains(this))
		{
			return World && !World->GetTimerManager().IsTimerActive(FCooldown) && Enemy->GetStamina() >= staminaCost;
		}
	}
	return false;
}

void UAbilityRainingArrows::Logic()
{
	if(!ProjectileToSpawn) return;
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter())) {
		UWorld* World = Enemy->GetWorld();
		if (!World) return;
		for (int i = 0; i < amountToSpawn; ++i)
		{
			FActorSpawnParameters params;
			params.Owner = Enemy;
			if (UMainGameInstance* GI = Cast<UMainGameInstance>(Enemy->GetGameInstance())) {
				if (GI->localPlayer) {
					float Rx = FMath::FRandRange(-CircleRadius, CircleRadius);
					float Ry = FMath::FRandRange(-CircleRadius, CircleRadius);
					FVector Random = FVector(Rx,Ry,0.f);
					FVector AroundPlayer = GI->localPlayer->GetActorLocation() + Random;
					FVector Location = AroundPlayer + (FVector::UpVector*300);
					UE_LOG(LogTemp, Warning, TEXT("Spawn at: %s"), *Location.ToString());
					if(AProjectile*proj = World->SpawnActor<AProjectile>(ProjectileToSpawn, Location, FRotator(0.f), params))
					{
						proj->LaunchProjectile(1.f, -proj->GetActorUpVector());
						if (World->GetTimerManager().IsTimerActive(FCooldown)) {
							World->GetTimerManager().ClearTimer(FCooldown);
						}
						World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
					}
				}
			}
		}
	}
}
