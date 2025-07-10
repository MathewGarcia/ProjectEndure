// Fill out your copyright notice in the Description page of Project Settings.


#include "HogCharge.h"

#include "AIEnemy.h"
#include "Enemy.h"
#include "MainGameInstance.h"
#include "Kismet/GameplayStatics.h"

class UMainGameInstance;

void UHogCharge::execute_Implementation()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		Rage = 0.f;
		Enemy->PlayAnimMontage(MontageToPlay, 1, "Charge");
		PlaySound();
		Enemy->GetWorldTimerManager().SetTimer(FRageTimerCooldown, [this]() {}, CooldownTime, false);
	}
}

bool UHogCharge::bShouldExecute_Implementation()
{
	return Rage >= 100.f;
}

void UHogCharge::Logic()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		if(!Enemy->GetWorldTimerManager().IsTimerActive(FRageTimerHandle))
		Enemy->GetWorldTimerManager().SetTimer(FRageTimerHandle, this, &UHogCharge::HandleRageLogic,RageTime,true);
	}
}

void UHogCharge::PlaySound()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(),SoundToPlay, Enemy->GetActorLocation(), Enemy->GetActorRotation(), 1, 1, 0, MGI->Attenuation);

		}
	}
}

void UHogCharge::HandleRageLogic()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		if(Enemy->Health <= 0)
		{
			Enemy->GetWorldTimerManager().ClearTimer(FRageTimerHandle);
			return;
		}

		if (AAIEnemy* EnemyController = Cast<AAIEnemy>(Enemy->GetController())) {
			if (Enemy->GetWorldTimerManager().IsTimerActive(FRageTimerCooldown) || EnemyController->CurrentState && EnemyController->CurrentState->GetState() == EAIStates::idle)
			{
				return;
			}
		}

		Rage += RageAmount;
		Rage = FMath::Clamp(Rage, 0.f, 100.f);
		if (bShouldExecute())
		{
			execute();
		}
	}
	
}
