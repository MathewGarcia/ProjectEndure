// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_Player_Ability_Used.generated.h"

class UAbility;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UANS_Player_Ability_Used : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, Category = "Frames")
	float TargetFrame;

	UPROPERTY(EditAnywhere, Category = "Frames")
	float TotalFrames = 1.f;


private:
	float CalculatedTime;
	bool bTriggered = false;
};
