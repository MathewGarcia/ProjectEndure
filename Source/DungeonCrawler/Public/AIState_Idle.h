// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIStateBase.h"
#include "Navigation/PathFollowingComponent.h"

/**
 * 
 */
class DUNGEONCRAWLER_API AIState_Idle : public AIStateBase
{
public:
	AIState_Idle();


	virtual void OnEnterState() override;

	virtual void OnExitState() override;

	virtual void TickState(float DeltaTime) override;

	virtual float CalculateOptions() override;

	virtual EAIStates GetState() override;

	virtual void ExecuteOption(float weight, AEnemy* Enemy) override;

	virtual TUniquePtr<AIDecisionTreeNode> BuildDecisionTree(AEnemy* Enemy) override;

	float currentDistance = 0.f;

	FVector NextLocation;

	int32 CurrentSplinePoint = 0;
	FTimerHandle FakeAttackTimer;

	void MoveToNextPoint();

	EPathFollowingRequestResult::Type Result;

};
