// Fill out your copyright notice in the Description page of Project Settings.


#include "AIState_Dead.h"

#include "AIEnemy.h"
#include "Enemy.h"

AIState_Dead::AIState_Dead()
{
}

void AIState_Dead::TickState(float DeltaTime)
{
	AIStateBase::TickState(DeltaTime);
}

void AIState_Dead::OnEnterState()
{
	if (world) {
		FTimerHandle destroyTimerHandle;

		if (AEnemy* Enemy = Cast<AEnemy>(AIEnemyController->GetPawn()))
		{
			Enemy->GetWorldTimerManager().ClearAllTimersForObject(Enemy);
			AIEnemyController->GetWorldTimerManager().ClearAllTimersForObject(AIEnemyController);

			TWeakObjectPtr<AEnemy> safeEnemy = Enemy;
			world->GetTimerManager().SetTimer(destroyTimerHandle, [this,safeEnemy]()
				{
				
					if (!safeEnemy.IsValid()) return;

					AEnemy* localEnemy = safeEnemy.Get();

					if (!localEnemy  || !IsValid(AIEnemyController)) return;
						localEnemy->Destroy();
						AIEnemyController->Destroy();


				}, 2.0, false);

		}

	}
	//AIStateBase::OnEnterState();
}

void AIState_Dead::OnExitState()
{
	AIStateBase::OnExitState();
}

EAIStates AIState_Dead::GetState()
{
	return EAIStates::dead;
}
