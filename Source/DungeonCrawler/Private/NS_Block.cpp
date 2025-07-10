// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_Block.h"

#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UNS_Block::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                            const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	bParryActive = true;
	if (APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		timeStarted = player->GetWorld()->GetTimeSeconds();
	}
}

void UNS_Block::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

}

void UNS_Block::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp) return;

	

	if (bParryActive) {

		if (APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
		{
			if(player->GetWorld()->GetTimeSeconds() - timeStarted > parryWindowTime)
			{
				bParryActive = false;
				return;
			}

			FVector Location = player->GetActorLocation() + (player->GetActorForwardVector() * 100);

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;
			ObjectTypeQuery.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(player);
			TArray<UPrimitiveComponent*> HitComponents;
			DrawDebugSphere(player->GetWorld(), Location, ParryRadius, 15, FColor::Red);
			if (UKismetSystemLibrary::SphereOverlapComponents(player->GetWorld(), Location, ParryRadius, ObjectTypeQuery, UCapsuleComponent::StaticClass(), ActorsToIgnore, HitComponents))
			{
				for (UPrimitiveComponent* component : HitComponents)
				{
					if (AEnemy* Enemy = Cast<AEnemy>(component->GetOwner()))
					{
						if (!Enemy->MiscMontages) return;

						if (Enemy->MiscMontages->IsValidSectionName("ParrySuccess")) {
							if (UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance())
							{
								if (AnimInstance->IsAnyMontagePlaying())
								{
									AnimInstance->StopAllMontages(0.f);
								}
							}
							FVector Direction = (Enemy->GetActorLocation() - player->GetActorLocation()).GetSafeNormal();
							Enemy->LaunchCharacter(Direction * 1000, true, true);
							player->SetParrySuccess(true);
							Enemy->PlayAnimMontage(Enemy->MiscMontages, 1.0, "ParrySuccess");
							UE_LOG(LogTemp, Warning, TEXT("parry success"));
						}
					}
				}
			}
		}
	}
}
