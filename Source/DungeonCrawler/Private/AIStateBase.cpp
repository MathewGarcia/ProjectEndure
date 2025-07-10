// Fill out your copyright notice in the Description page of Project Settings.


#include "AIStateBase.h"

#include "AIEnemy.h"
#include "Enemy.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"

AIStateBase::AIStateBase()
{
	DecisionTree = MakeUnique<AIDecisionTree>();
}

AIStateBase::~AIStateBase()
{
	
}

void AIStateBase::OnEnterState()
{
	if(!bIsEnabled)
	{
		return;
	}
	if (AIEnemyController) {
		TWeakObjectPtr<AAIEnemy> SafeAIEnemyController = AIEnemyController;
		world->GetTimerManager().SetTimer(ExecuteTimerHandle, [this,SafeAIEnemyController]()
			{
				if (!SafeAIEnemyController.IsValid()) return;

				AAIEnemy* LocalAIEnemyController = SafeAIEnemyController.Get();
				if (AEnemy* Enemy = Cast<AEnemy>(LocalAIEnemyController->GetPawn()))
				{
					if (Enemy->GetMesh() && Enemy->GetMesh()->GetAnimInstance() && !Enemy->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {
						DecisionTree->Evaluate(LocalAIEnemyController->features, Enemy);
					}

				}
			}, .3, true);
	}

}

void AIStateBase::TickState(float DeltaTime)
{


	if (!PlayerWeakPtr.IsValid())
	{
		return;
	}

	APlayerCharacter*player = PlayerWeakPtr.Get();

	if (!AIEnemyController || !player) return;


		if (AEnemy* Enemy = Cast<AEnemy>(AIEnemyController->GetPawn())) {

			if (world)
			{

				PlayerDistance = (player->GetActorLocation() - Enemy->GetActorLocation()).Size();
			}
		}
	
}

void AIStateBase::OnExitState()
{
	if (world->GetTimerManager().IsTimerActive(ExecuteTimerHandle))
	{
		world->GetTimerManager().ClearTimer(ExecuteTimerHandle);
	}
}

float AIStateBase::CalculateOptions()
{
	return 0;
}


EAIStates AIStateBase::GetState()
{
	return EAIStates::none;
}

void AIStateBase::ExecuteOption(float weight,AEnemy*Enemy)
{
}

TUniquePtr<AIDecisionTreeNode> AIStateBase::BuildDecisionTree(AEnemy* Enemy)
{
	TUniquePtr<AIDecisionTreeNode> Root = MakeUnique<AIDecisionTreeNode>();

	Root->featureIndex = 0;
	Root->NodeType = ENodeType::Decision;

	if(Enemy){
		Root->SplitValue = Enemy->AttackDistance*15.f;
	}
	else
	{
		Root->SplitValue = 400.f;
	}

	return Root;
}

TUniquePtr<AIDecisionTreeNode> AIStateBase::BuildInRange(AEnemy* Enemy)
{
	return nullptr;
}

TUniquePtr<AIDecisionTreeNode> AIStateBase::BuildNotInRange(AEnemy* Enemy)
{
	return nullptr;
}


bool AIStateBase::IsPlayerFacingAway(AEnemy* Enemy)
{

	if (!PlayerWeakPtr.IsValid() || !IsValid(Enemy)) return false;

	APlayerCharacter* player = PlayerWeakPtr.Get();

	FVector ToPlayer = (Enemy->GetActorLocation() - player->GetActorLocation()).GetSafeNormal();
	float result = FVector::DotProduct(player->GetActorForwardVector(), ToPlayer);
	float threshold = FMath::Cos(FMath::RadiansToDegrees(-45.f));

	if (result < threshold)
	{
		return true;
	}
	return false;
}

bool AIStateBase::IsPlayerStunned()
{
	return false;
}

bool AIStateBase::IsPlayerAttacking()
{
	return false;
}

void AIStateBase::SetEnabled(bool bEnabled)
{
	bIsEnabled = bEnabled;
	if (!world) return;
	if(!bIsEnabled)
	{
		world->GetTimerManager().ClearAllTimersForObject(this);
	}
	else if(bIsEnabled && !world->GetTimerManager().IsTimerActive(ExecuteTimerHandle))
	{
		if (AIEnemyController) {
			TWeakObjectPtr<AAIEnemy> SafeAIEnemyController = AIEnemyController;

			world->GetTimerManager().SetTimer(ExecuteTimerHandle, [this,SafeAIEnemyController]()
				{
					if (!SafeAIEnemyController.IsValid()) return;

					AAIEnemy* LocalEnemyController = SafeAIEnemyController.Get();
					if (AEnemy* Enemy = Cast<AEnemy>(LocalEnemyController->GetPawn()))
					{
						if (Enemy->GetMesh() && Enemy->GetMesh()->GetAnimInstance() && !Enemy->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {
							DecisionTree->Evaluate(LocalEnemyController->features, Enemy);
						}

					}
				}, .3, true);
		}
	}
}


