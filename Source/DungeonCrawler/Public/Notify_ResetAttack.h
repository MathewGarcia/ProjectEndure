// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Notify_ResetAttack.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UNotify_ResetAttack : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "IsDodge")
	bool bIsDodge = false;
protected:

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
