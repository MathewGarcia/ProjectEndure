// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PlayGroundSound.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UPlayGroundSound : public UAnimNotify
{
	GENERATED_BODY()
	

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
