// Fill out your copyright notice in the Description page of Project Settings.


#include "ResetHitActors.h"

#include "PlayerCharacter.h"
#include "Weapon.h"

void UResetHitActors::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if(APlayerCharacter*player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		if(AWeapon*AttackingWeapon = player->GetAttackingWeapon())
		{
			AttackingWeapon->HitActors.Empty();
		}
	}
}
