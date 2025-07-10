#include "WaterDebuff.h"
#include "Enemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWaterDebuff::ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser)
{
	if(APlayerCharacter*player = Cast<APlayerCharacter>(Target))
	{
		player->GetCharacterMovement()->MaxWalkSpeed = 300.f;

		UNiagaraComponent*NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, player->GetMesh(), "ChestGear", FVector(0.f), FRotator(0.f), EAttachLocation::Type::SnapToTarget, false, true);

		TWeakObjectPtr<APlayerCharacter> safePlayer = player;

		player->GetWorldTimerManager().SetTimer(FDebuffTimerHandle, [this, safePlayer,NiagaraComponent]()
		{
				if (APlayerCharacter* localPlayer = safePlayer.Get()) {

					localPlayer->GetCharacterMovement()->MaxWalkSpeed = 600.f;
					if (NiagaraComponent)
					{
						NiagaraComponent->DestroyComponent();
					}
				}

		}, DebuffTime, false);
	}
	else if(AEnemy*enemy = Cast<AEnemy>(Target))
	{
		enemy->GetCharacterMovement()->MaxWalkSpeed = 300.f;

		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, enemy->GetMesh(), "spine_01", FVector(0.f), FRotator(0.f), EAttachLocation::Type::SnapToTarget, false, true);

		TWeakObjectPtr<AEnemy> safeEnemy = enemy;
		enemy->GetWorldTimerManager().SetTimer(FDebuffTimerHandle, [this, safeEnemy,NiagaraComponent]()
			{
				if (!safeEnemy.IsValid()) return;
				if (AEnemy* localEnemy = safeEnemy.Get()) {
					localEnemy->GetCharacterMovement()->MaxWalkSpeed = 600.f;
					if (NiagaraComponent)
					{
						NiagaraComponent->DestroyComponent();
					}
				}
			}, DebuffTime, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("Water debuff"));
}

void UWaterDebuff::EndBuff()
{
	Super::EndBuff();
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
	{
		player->CurrentDebuffs.Remove(EElementTypes::Water);
	}
	else if (AEnemy* enemy = Cast<AEnemy>(GetOuter()))
	{
		enemy->CurrentDebuffs.Remove(EElementTypes::Water);
	}
	
}
