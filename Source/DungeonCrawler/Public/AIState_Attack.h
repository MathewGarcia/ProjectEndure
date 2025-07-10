// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIStateBase.h"

/**
 * 
 */
class DUNGEONCRAWLER_API AIState_Attack : public AIStateBase
{
public:

	virtual void OnEnterState() override;

	virtual void OnExitState() override;

	virtual void TickState(float DeltaTime) override;

	virtual float CalculateOptions() override;

	virtual EAIStates GetState() override;

	virtual TUniquePtr<AIDecisionTreeNode> BuildInRange(AEnemy* Enemy) override;

	virtual TUniquePtr<AIDecisionTreeNode> BuildNotInRange(AEnemy* Enemy) override;


	bool IsInRange = false;
	virtual void ExecuteOption(float weight,AEnemy*Enemy) override;

	virtual TUniquePtr<AIDecisionTreeNode> BuildDecisionTree(AEnemy* Enemy) override;
};
