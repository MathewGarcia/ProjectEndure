// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityRainingArrows.h"
#include "PlayerCharacter.h"
#include "Enemy.h"
#include "MainGameInstance.h"

void UAbilityRainingArrows::execute_Implementation()
{

	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		Enemy->PlayAnimMontage(Enemy->MiscMontages, 1, "RainingArrows");

	}
	Super::execute_Implementation();
}

bool UAbilityRainingArrows::bShouldExecute_Implementation()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		if (Enemy->EnemyParameters.OffensiveAbilities.Contains(this))
		{
			return !GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && Enemy->GetStamina() >= staminaCost;
		}
	}
	return false;
}

void UAbilityRainingArrows::Logic()
{
	if(ProjectileToSpawn)
	{
		if (AEnemy* Enemy = Cast<AEnemy>(GetOuter())) {
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
						FVector Location = AroundPlayer	 + (FVector::UpVector*300);
						UE_LOG(LogTemp, Warning, TEXT("Spawn at: %s"), *Location.ToString());

						if(AProjectile*proj = Enemy->GetWorld()->SpawnActor<AProjectile>(ProjectileToSpawn, Location, FRotator(0.f), params))
						{
							proj->LaunchProjectile(1.f, -proj->GetActorUpVector());
							GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
						}
					}
				}
			}
		}
	}
}
