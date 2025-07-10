// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIDecisionTree.h"
#include "AIDecisionTreeNode.h"
#include "AIStates.h"

/**
 * 
 */

class APlayerCharacter;
class AEnemy;
class AAIEnemy;
class DUNGEONCRAWLER_API AIStateBase
{
public:
	AIStateBase();
	virtual ~AIStateBase();

	virtual void OnEnterState();
	
	virtual void TickState(float DeltaTime);

	virtual void OnExitState();

	virtual float CalculateOptions();

	bool bCanCalculate = true;

	virtual EAIStates GetState();

	virtual void ExecuteOption(float weight,AEnemy*Enemy);

	virtual TUniquePtr<AIDecisionTreeNode> BuildDecisionTree(AEnemy* Enemy);

	virtual TUniquePtr<AIDecisionTreeNode> BuildInRange(AEnemy* Enemy);

	virtual TUniquePtr<AIDecisionTreeNode> BuildNotInRange(AEnemy* Enemy);

	FTimerHandle ExecuteTimerHandle;

	bool IsPlayerFacingAway(AEnemy* Enemy);

	bool IsPlayerStunned();

	bool IsPlayerAttacking();

	float GetPlayerDistance() const
	{
		return PlayerDistance;
	}

	bool bIsEnabled = true;

	void SetEnabled(bool bEnabled);

	TUniquePtr<AIDecisionTree> DecisionTree;

	AAIEnemy* AIEnemyController;
	UWorld* world;

	TWeakObjectPtr<APlayerCharacter> PlayerWeakPtr;

protected:
	float PlayerDistance = 10000.f;

};
