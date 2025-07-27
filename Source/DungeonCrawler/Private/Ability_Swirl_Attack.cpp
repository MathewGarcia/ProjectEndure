// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Swirl_Attack.h"
#include "Weapon.h"
#include "Enemy.h"
#include "PlayerCharacterState.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void UAbility_Swirl_Attack::execute_Implementation()
{
	Super::execute_Implementation();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (MontageToPlay)
				player->PlayAnimMontage(MontageToPlay, 1.f, "Swirl");
		}
	}
}

bool UAbility_Swirl_Attack::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			UWorld* World = PCS->GetWorld();
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && World && !World->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost);
		}
	}
	return Super::bShouldExecute_Implementation();
}

void UAbility_Swirl_Attack::Logic()
{
	Super::Logic();
	TArray angles = { -15.f,-7.5f,0.f,7.5f, 15.f };
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			UWorld* World = player->GetWorld();
			if (!World) return;
			if (AWeapon* Weapon = player->GetCurrentWeapon()) {
				if (Weapon->WeaponType == EWeaponType::OneHand || Weapon->WeaponType == EWeaponType::TwoHand) {
					Weapon->SetIsSwinging(true);
					TArray<AActor*> actorsToIgnore;
					actorsToIgnore.Add(player);
					actorsToIgnore.Add(player->GetCurrentWeapon());
					float ClosestEnemyDistance = 100000;
					AEnemy* ClosestEnemy = nullptr;
					for (auto angle : angles) {
						if (!player->Cam) continue;
						FVector Direction = player->Cam->GetForwardVector().RotateAngleAxis(angle, player->GetActorUpVector());
						FVector End = player->GetActorLocation() + Direction * 1000.f;
						FHitResult Hit;
						if (UKismetSystemLibrary::SphereTraceSingle(World, player->GetActorLocation(), End, 50.0, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, actorsToIgnore, EDrawDebugTrace::Persistent, Hit, true, FLinearColor::Red, FLinearColor::Green, 25.f))
						{
							if (AEnemy* enemy = Cast<AEnemy>(Hit.GetActor()))
							{
								float Distance = FVector::Dist(player->GetActorLocation(), enemy->GetActorLocation());
								if (Distance < ClosestEnemyDistance)
								{
									ClosestEnemyDistance = Distance;
									ClosestEnemy = enemy;
								}
							}
						}
					}
					FVector FinalCalc;
					if (ClosestEnemy)
					{
						FVector ToEnemy = ClosestEnemy->GetActorLocation() - player->GetActorLocation();
						FVector DirectionToEnemy = ToEnemy.GetSafeNormal();
						float DistanceToEnemy = ToEnemy.Size();
						FinalCalc = player->GetActorLocation() + (DirectionToEnemy * DistanceToEnemy);
					}
					else
					{
						FinalCalc = player->GetActorLocation() + (player->GetActorForwardVector() * 1000.f);
					}
					player->SetActorLocation(FinalCalc, true);
					if (World->GetTimerManager().IsTimerActive(FCooldown)) {
						World->GetTimerManager().ClearTimer(FCooldown);
					}
					World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
				}
			}
		}
	}
}

void UAbility_Swirl_Attack::OnTriggered()
{
	Super::OnTriggered();

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (AWeapon* Weapon = player->GetCurrentWeapon()) {
				if (Weapon->WeaponType == EWeaponType::OneHand || Weapon->WeaponType == EWeaponType::TwoHand) {
					Weapon->SetIsSwinging(false);
				}
			}
		}
	}
}
