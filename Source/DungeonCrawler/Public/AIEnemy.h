// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIDecisionTree.h"
#include "AIState_Attack.h"
#include "AIState_Idle.h"
#include "AIState_Defense.h"
#include "AIState_Dead.h"
#include "Enemy.h"
#include "AIEnemy.generated.h"

class APlayerCharacterState;
class AIState_Idle;
class AIState_Attack;
class AIState_Defense;
class AIStateBase;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API AAIEnemy : public AAIController
{
	GENERATED_BODY()

public:

	AAIEnemy();

	AIStateBase* CurrentState = nullptr;

	void ChangeAIState(AIStateBase* AIState);


	TUniquePtr<AIState_Attack> AttackState;

	TUniquePtr<AIState_Idle> IdleState;

	TUniquePtr<AIState_Defense> DefenseState;

	TUniquePtr<AIState_Dead> DeadState;

	TArray<float>features;

	FTimerHandle ReadyCheckHandle;
	FTimerHandle CheckForNearbyEnemies;


	void SetStates(EEnemyType EnemyType);

	UPROPERTY()
	APlayerCharacterState* PCS;

	UPROPERTY()
	APlayerCharacter* Player = nullptr;

	
	UPROPERTY()
	AEnemy* Enemy = nullptr;

	FTimerHandle FBeginPlayTimer;

	bool bStatesReady() const;

	FTimerHandle FWanderTimerHandle;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	void SetAIEnabled(bool bEnabled);

	FTimerHandle FResetMovementCompleted;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossess(APawn* InPawn) override;

};
