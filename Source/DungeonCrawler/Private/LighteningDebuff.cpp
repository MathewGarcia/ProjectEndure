// Fill out your copyright notice in the Description page of Project Settings.


#include "LighteningDebuff.h"

#include "Enemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "Weapon.h"
#include "Engine/DamageEvents.h"

void ULighteningDebuff::ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser)
{
	Super::ExecuteDebuff_Implementation(Target,DamageCauser);

	if (APlayerCharacter* player = Cast<APlayerCharacter>(Target))
	{
		if(NiagaraSystem)
		{
			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(player->GetWorld(), NiagaraSystem, player->GetActorLocation() + player->GetActorUpVector() * 50.f , FRotator(0.f));
		}
		player->PlayerStunned();

		if(AEnemy*enemy = Cast<AEnemy>(DamageCauser))
		{

			player->TakeDamage(enemy->EnemyParameters.ElementDamage * 2, FDamageEvent(), nullptr, nullptr);
			
		}
	}
	else if (AEnemy* enemy = Cast<AEnemy>(Target))
	{

		if (NiagaraSystem)
		{
			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(enemy->GetWorld(), NiagaraSystem, enemy->GetActorLocation()+ enemy->GetActorUpVector() *50.f, FRotator(0.f));
		}

		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(DamageCauser)) {
			if (AWeapon* Weapon = PlayerCharacter->GetCurrentWeapon())
			{
				if (Weapon->ItemData) {
					if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(Weapon->ItemData)) {
						PlayerCharacter->TakeDamage(WeaponItemData->ElementTypeDamage * 2, FDamageEvent(), nullptr, nullptr);
					}
				}
			}
		}
		//stun enemy.
	}
}
