// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss_Wave.h"
#include "AIEnemy.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"
#include "Engine/DamageEvents.h"


void ABoss_Wave::SpawnEnemy()
{

	if (CurrentEnemies.Num() >= amountToSpawn) return;

	if(ChosenEnemy)
	{
		FHitResult HitResult;
		FVector Start = GetActorLocation() + FVector(FMath::FRandRange(-MaxRange,MaxRange),FMath::FRandRange(-MaxRange,MaxRange),0);
		FVector End = Start + (FVector::DownVector * 1000.f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic)) {
			if (HitResult.GetActor()->ActorHasTag("SpawnSurface")) {
				FActorSpawnParameters params;
				params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				if (AEnemy* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(ChosenEnemy,HitResult.ImpactPoint,FRotator::ZeroRotator,params))
				{
					if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(SpawnedEnemy->GetController()))
					{
						AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get());
						CurrentEnemies.Add(SpawnedEnemy);

					}
				}
			}
		}
	}
}


void ABoss_Wave::StartBoss()
{
	GetWorld()->GetTimerManager().SetTimer(FSpawnRateTimerHandle, this, &ABoss_Wave::SpawnEnemy, SpawnRateTime, true);

}

void ABoss_Wave::Destroyed()
{
	Super::Destroyed();

	for (int i = CurrentEnemies.Num() - 1; i >= 0; --i)
	{
		if (IsValid(CurrentEnemies[i]))
		{
			CurrentEnemies[i]->Destroy();
		}
	}
	CurrentEnemies.Empty();
}

float ABoss_Wave::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{

	Health -= DamageAmount;
	Health = FMath::Clamp(Health, 0, MaxHealth);
	UpdateBossHealthUI();

	if(Health <= 0)
	{
		if (!bDead) {
			bDead = true;

			AEnemy::OnEnemyDeath.Broadcast(this);
			InstancedOnEnemyDeath.Broadcast(this);

			if (APlayerCharacter* player = Cast<APlayerCharacter>(DamageCauser))
			{
				if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(player->GetPlayerState()))
				{
					pcs->playerStats.UpdateEXP(EnemyParameters.Exp);
					StopDebuffs();
					HideBossHealthBarUI(pcs);
					SpawnBossBuff();
					UpdatePlayerEXPUI(pcs);
				}
			}

			GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
			Destroy();

		}
	}

	return APawn::TakeDamage(DamageAmount, FDamageEvent(), EventInstigator, DamageCauser);
}

TSubclassOf<AEnemy> ABoss_Wave::RollForEnemy()
{
	float Total = 0.f;

	for(const auto& pair : EnemiesClasses)
	{
		Total += pair.Value;
	}

	float Roll = FMath::FRandRange(0, Total);

	for (const auto& pair : EnemiesClasses)
	{
		Roll -= pair.Value;

		if(Roll <= 0)
		{
			return pair.Key;
		}
	}

	return nullptr;
}

void ABoss_Wave::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!CurrentEnemies.IsEmpty()) {

		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
			if (APlayerCharacter* player = MGI->localPlayer) {
				for (int i = CurrentEnemies.Num() - 1; i >= 0; --i)
				{
						if (!IsValid(CurrentEnemies[i]))
						{
							TakeDamage(10, FDamageEvent(), player->GetController(), player);
							if(bDead)
							{
								return;
							}

							CurrentEnemies.RemoveAt(i);
							break;
						}
				}
			}
		}
	}
}

void ABoss_Wave::BeginPlay()
{
	Super::BeginPlay();

	ChosenEnemy = RollForEnemy();

	if(int32* localAmountToSpawn = AmountMap.Find(ChosenEnemy))
	{
		amountToSpawn = *localAmountToSpawn;
	}
}
