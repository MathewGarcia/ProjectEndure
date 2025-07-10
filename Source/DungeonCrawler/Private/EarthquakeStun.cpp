// Fill out your copyright notice in the Description page of Project Settings.


#include "EarthquakeStun.h"

#include "Enemy.h"
#include "EnvironmentalItem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

void UEarthquakeStun::execute_Implementation()
{
	Super::execute_Implementation();

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter())) {
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			player->SetPlayerState(PlayerStates::ABILITY);
			player->PlayAnimMontage(MontageToPlay);
		}
	}
	
}

bool UEarthquakeStun::bShouldExecute_Implementation()
{
	//do we have enough mana, etc.

	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && !PCS->GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost);
		}
	}

	return false;
}

void UEarthquakeStun::Logic()
{
	Super::Logic();


	TArray<float> Directions = { -15.f,-7.5f,0.f,7.5f, 15.f };
	
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter())) {
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {

			Timers.Empty();
			Timers.SetNum(Directions.Num());

			TArray<AActor*>HitActors;

			for (int i = 0; i < Directions.Num(); ++i)
			{
				FRotator Rot = FRotator(0.f, Directions[i], 0.f);
				FVector RotatedDirection = Rot.RotateVector(player->GetActorForwardVector());

				TWeakObjectPtr<APlayerCharacter> SafePlayer = player;



				float Step = 0.f;
				float Max = 1000.f;
				FVector StartLocation = player->GetActorLocation();
				FTimerHandle& LocalHandle = Timers[i];
				GetWorld()->GetTimerManager().SetTimer(LocalHandle, [this,i,SafePlayer, Step, Max, RotatedDirection, StartLocation,HitActors]() mutable
					{
						if (!SafePlayer.IsValid()) return;

						APlayerCharacter* localPlayer = SafePlayer.Get();

						if (!localPlayer) return;

						Step += 100.f;
						FVector CurrentLocation = StartLocation + (RotatedDirection * Step);

						UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, CurrentLocation, FRotator::ZeroRotator);

						TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
						TArray<AActor*> ActorsToIgnore;
						TArray<AActor*> OutActors;
						ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
						if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), CurrentLocation, 500.f, ObjectTypes, AEnemy::StaticClass(), ActorsToIgnore, OutActors))
						{
							for (AActor* ActorEnemy : OutActors)
							{
								if (AEnemy* HitEnemy = Cast<AEnemy>(ActorEnemy))
								{
									if(HitActors.Contains(HitEnemy))
									{
										continue;
									}

									HitActors.Add(HitEnemy);

									HitEnemy->TakeDamage(100.f, FDamageEvent(), localPlayer->GetController(), localPlayer);
									HitEnemy->PlayAnimMontage(HitEnemy->MiscMontages, 1.0, "ParrySuccess");
								}
							}
						}

						if (Step >= Max)
						{
							if (Timers.IsValidIndex(i)) {
								GetWorld()->GetTimerManager().ClearTimer(Timers[i]);
							}
						}


					}, 0.1, true);

				GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
			}
		}
	}
}
