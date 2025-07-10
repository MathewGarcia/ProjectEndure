// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Salamander_Dig.h"

#include "AIEnemy.h"
#include "Enemy.h"
#include "MainGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"



void UAbility_Salamander_Dig::execute_Implementation()
{
	Super::execute_Implementation();

	if(AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
	{
		Enemy->PlayAnimMontage(MontageToPlay);
		bIsActive = true;
	}

}

void UAbility_Salamander_Dig::Logic()
{
	Super::Logic();

	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (APlayerCharacter* player = MGI->localPlayer)
		{
			if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
			{
				if(Enemy->GetCharacterMovement())
				{
					Enemy->GetCharacterMovement()->MaxWalkSpeed = 1200;
				}
				FTimerHandle FSmallDelay;
				Enemy->bMovementCompleted = false;
				GetWorld()->GetTimerManager().SetTimer(FSmallDelay, [this,player,Enemy]() 
				{
						FVector Location = player->GetActorLocation();
						if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(Enemy->GetController())) {
							if (JumpoutDelegateHandle.IsValid()) {
								AIEnemyController->GetPathFollowingComponent()->OnRequestFinished.Remove(JumpoutDelegateHandle);
							}
								JumpoutDelegateHandle = AIEnemyController->GetPathFollowingComponent()->OnRequestFinished.AddUFunction(this, "JumpOut");
							AIEnemyController->MoveToLocation(Location, 1.f, false, true, true, false, 0, true);

						}
				},2.f,false);

				TWeakObjectPtr<AEnemy> SafeEnemy= Enemy;
				GetWorld()->GetTimerManager().SetTimer(FUndergroundShake, [this,SafeEnemy]
				{
						if (!SafeEnemy.IsValid()) return;
						if (AEnemy* localEnemy = SafeEnemy.Get()) {
							UGameplayStatics::PlayWorldCameraShake(
								GetWorld(),
								UndergroundCameraShake,
								localEnemy->GetActorLocation(),
								0.f,
								99999.f,
								2.5f
							);

							UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystemToPlay, localEnemy->GetActorLocation(), FRotator::ZeroRotator);
							UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay,localEnemy->GetActorLocation(),localEnemy->GetActorRotation());
						}

				}, 0.5, true);
			}
		}
	}



	GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
}

bool UAbility_Salamander_Dig::bShouldExecute_Implementation()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		if (Enemy->EnemyParameters.OffensiveAbilities.Contains(this))
		{
			return !bIsActive && !GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && Enemy->GetStamina() >= staminaCost;
		}
	}
	return false;
}

void UAbility_Salamander_Dig::JumpOut()
{

	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (APlayerCharacter* player = MGI->localPlayer)
		{
			if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
			{

				if (!Enemy->bMovementCompleted) return;
				GetWorld()->GetTimerManager().ClearTimer(FUndergroundShake);
				UGameplayStatics::PlayWorldCameraShake(
					GetWorld(),
					BurstingCameraShake,
					Enemy->GetActorLocation(),
					0.f,
					99999.f,
					2.5f
				);
				Enemy->PlayAnimMontage(MontageToPlay, 1.f, "Dig_End");
				TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
				ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(Enemy);
				TArray<AActor*>OutActors;
				if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Enemy->GetActorLocation(), 1000.f, ObjectTypes, player->GetClass(), ActorsToIgnore, OutActors))
				{
					for (AActor* Actor : OutActors)
					{
						if (APlayerCharacter* playerHit = Cast<APlayerCharacter>(Actor))
						{
							playerHit->TakeDamage(Enemy->Damage, FDamageEvent(), Enemy->GetController(), Enemy);
							playerHit->PlayerStunned();

						}
					}
				}

				if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(Enemy->GetController())) {
					if (JumpoutDelegateHandle.IsValid()) {
						AIEnemyController->GetPathFollowingComponent()->OnRequestFinished.Remove(JumpoutDelegateHandle);
						JumpoutDelegateHandle.Reset();
					}
					if (Enemy->GetCharacterMovement())
					{
						Enemy->GetCharacterMovement()->MaxWalkSpeed = 600.f;
					}
				}
				bIsActive = false;
			}
		}
	}
}
