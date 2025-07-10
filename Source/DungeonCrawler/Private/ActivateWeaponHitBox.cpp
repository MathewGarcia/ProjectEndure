// Fill out your copyright notice in the Description page of Project Settings.


#include "ActivateWeaponHitBox.h"

#include "PlayerCharacter.h"

#include "PlayerCharacterState.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"

void UActivateWeaponHitBox::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;
	 
	if (APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(player->GetPlayerState()))
		{
			if(AItem** FoundWeapon = PCS->PlayerGear.Find(EGearType::RHand))
			{
				if (AWeapon* Weapon = Cast<AWeapon>(*FoundWeapon)) {
					Weapon->HitActors.Empty();
					Weapon->Swing();
				}

			}
		}
	}

}

void UActivateWeaponHitBox::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{

	Super::NotifyEnd(MeshComp, Animation, EventReference);


	if (!MeshComp) return;

	if (APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{

		if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(player->GetPlayerState()))
		{
			if (AWeapon* Weapon = Cast<AWeapon>(PCS->PlayerGear[EGearType::RHand]))
			{
				/*Weapon->CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
				Weapon->CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);*/
				Weapon->SetIsSwinging(false);

			}
		}

	}


}
