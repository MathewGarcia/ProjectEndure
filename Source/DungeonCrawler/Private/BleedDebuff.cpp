// Fill out your copyright notice in the Description page of Project Settings.


#include "BleedDebuff.h"

#include "Enemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/DamageEvents.h"
#include "PlayerCharacter.h"
#include "Weapon.h"
#include "PlayerCharacterState.h"

void UBleedDebuff::ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser)
{
	Super::ExecuteDebuff_Implementation(Target,DamageCauser);

    if (!Target) return;

	if (APlayerCharacter* player = Cast<APlayerCharacter>(Target))
	{

		if(player->CurrentDebuffs.Find(EElementTypes::Bleed))
		{
			return;
		}

		player->CurrentDebuffs.Add(EElementTypes::Bleed, this);

		if(AEnemy*enemy = Cast<AEnemy>(DamageCauser))
		{
			player->TakeDamage(enemy->EnemyParameters.ElementDamage*2, FDamageEvent(), enemy->GetController(), enemy);
		}
		else
		{
			if (NiagaraSystem && player->GetWorld()) {
				UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(player->GetWorld(), NiagaraSystem, player->GetActorLocation());
			}

			player->TakeDamage(50, FDamageEvent(), nullptr, nullptr);

		}
		player->CurrentDebuffs.Remove(EElementTypes::Bleed);

	}
	else if (AEnemy* enemy = Cast<AEnemy>(Target))
	{


		if (enemy->CurrentDebuffs.Find(EElementTypes::Bleed))
		{
			return;
		}

		enemy->CurrentDebuffs.Add(EElementTypes::Bleed, this);
		if (NiagaraSystem && enemy->GetWorld()) {
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(enemy->GetWorld(), NiagaraSystem, enemy->GetActorLocation());
		}

			if(AWeapon*Weapon = Cast<AWeapon>(DamageCauser))
			{
				UE_LOG(LogTemp, Warning, TEXT("ENEMY BLEED"));
				if (Weapon->ItemData) {
					if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(Weapon->ItemData)) {
						enemy->TakeDamage(WeaponItemData->ElementTypeDamage * 2, FDamageEvent(), nullptr, Weapon->GetOwner());
					}
					enemy->CurrentDebuffs.Remove(EElementTypes::Bleed);
				}
			}


	}
}
