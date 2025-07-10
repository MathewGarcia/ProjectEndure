// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Throw_Weapon.h"

#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Weapon.h"
#include "PlayerCharacterState.h"
#include "Kismet/KismetSystemLibrary.h"

void UAbility_Throw_Weapon::execute_Implementation()
{
	Super::execute_Implementation();

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			player->PlayAnimMontage(MontageToPlay, 1.f, "Throw");
		}

	}
}

bool UAbility_Throw_Weapon::bShouldExecute_Implementation()
{

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && !PCS->GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost);
		}
	}
	return Super::bShouldExecute_Implementation();
}

void UAbility_Throw_Weapon::Logic()
{
	Super::Logic();

	TArray angles = { -15.f,-7.5f,0.f,7.5f, 15.f };

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (AWeapon* Weapon = player->GetCurrentWeapon())
			{
				if (Weapon->WeaponType == EWeaponType::OneHand || Weapon->WeaponType == EWeaponType::TwoHand)
				{
					TArray<AActor*> actorsToIgnore;

					actorsToIgnore.Add(player);
					actorsToIgnore.Add(player->GetCurrentWeapon());

					float ClosestEnemyDistance = 100000;
					AEnemy* ClosestEnemy = nullptr;
					for (auto angle : angles) {
						FVector Direction = player->Cam->GetForwardVector().RotateAngleAxis(angle, player->GetActorUpVector());
						FVector End = player->GetActorLocation() + Direction * 1000.f;
						FHitResult Hit;
						if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), player->GetActorLocation(), End, 50.0, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, actorsToIgnore, EDrawDebugTrace::Persistent, Hit, true, FLinearColor::Red, FLinearColor::Green, 25.f))
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
					Weapon->ThrowWeapon(player,ClosestEnemy);
					GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);

				}
			}
		}
	}
}

void UAbility_Throw_Weapon::OnTriggered()
{
	Super::OnTriggered();


	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (AWeapon* Weapon = player->GetCurrentWeapon())
			{
				Weapon->ReturnToPlayer();
			}
		}

	}
}
