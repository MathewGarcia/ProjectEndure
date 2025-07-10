// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NS_EnemyAttack.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UNS_EnemyAttack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Collision Name")
	TArray<FName> CollisionNames;

	UPROPERTY(EditAnywhere, Category = "Should stun")
	bool bShouldStun = false;

	UPROPERTY(EditAnywhere,Category = "Projectile")
	bool bProjectile = false;

	UPROPERTY(EditAnywhere, Category = "Ability used?")
	bool AbilityUsed = false;


	UPROPERTY(EditAnywhere, Category = "Ability used?", meta = (EditCondition = "AbilityUsed", EditConditionHides))
	int32 AbilityPos = 0;

	UPROPERTY(EditAnywhere, Category = "Ability used?", meta = (EditCondition = "AbilityUsed", EditConditionHides))
	EAbilityType AbilityType;
protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
