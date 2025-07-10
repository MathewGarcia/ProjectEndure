#include "AIState_Salamander_Attack.h"
#include "PlayerCharacter.h"
#include "AIEnemy.h"
#include "Enemy.h"

TUniquePtr<AIDecisionTreeNode> AIState_Salamander_Attack::BuildInRange(AEnemy* Enemy)
{
	//feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is enemy stamina, 4 is player state, 5 is montage time, 6 is player facing away?

	/*
	 *				(EnemyStamina)
	 *				/			\
	 *			(roam)			(check passivitiy)
	 *								/		\
	 *							(roam)		(attack)
	 */

	TUniquePtr<AIDecisionTreeNode> EnemyStamina = DecisionTreeHelper::MakeDecisionNode(2,Enemy->AttackStaminaReduction);
	TUniquePtr<AIDecisionTreeNode> Passivity = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
	TUniquePtr<AIDecisionTreeNode> Attack = DecisionTreeHelper::MakeLeafNode([this,Enemy]
	{
			if (!Enemy) return;
			Enemy->Attack();
	});

	TUniquePtr<AIDecisionTreeNode> Wait1 = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{
			if (!PlayerWeakPtr.IsValid()) return;

			if (APlayerCharacter* player = PlayerWeakPtr.Get())
			{
				if (Enemy->bUseControllerRotationYaw)
				{
					Enemy->bUseControllerRotationYaw = false;
				}

				FVector DirectionToPlayer = (player->GetActorLocation() - Enemy->GetActorLocation());
				FVector StrafeDirection = FVector::CrossProduct(DirectionToPlayer.GetSafeNormal(), FVector::UpVector);
				FVector TargetLocation = Enemy->GetActorLocation() + StrafeDirection * FMath::FRandRange(-500.f, 500.f);
				Enemy->FacePlayer();
				AIEnemyController->MoveToLocation(TargetLocation, 1.f);

			}
		});

	TUniquePtr<AIDecisionTreeNode> Wait2 = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{
			if (!PlayerWeakPtr.IsValid()) return;

			if (APlayerCharacter* player = PlayerWeakPtr.Get())
			{
				if (Enemy->bUseControllerRotationYaw)
				{
					Enemy->bUseControllerRotationYaw = false;
				}

				FVector DirectionToPlayer = (player->GetActorLocation() - Enemy->GetActorLocation());
				FVector StrafeDirection = FVector::CrossProduct(DirectionToPlayer.GetSafeNormal(), FVector::UpVector);
				FVector TargetLocation = Enemy->GetActorLocation() + StrafeDirection * FMath::FRandRange(-500.f, 500.f);
				Enemy->FacePlayer();
				AIEnemyController->MoveToLocation(TargetLocation, 1.f);

			}
		});

	Passivity->RightChild = MoveTemp(Attack);
	Passivity->LeftChild = MoveTemp(Wait1);

	EnemyStamina->RightChild = MoveTemp(Passivity);
	EnemyStamina->LeftChild = MoveTemp(Wait2);

	return EnemyStamina;
}

TUniquePtr<AIDecisionTreeNode> AIState_Salamander_Attack::BuildNotInRange(AEnemy* Enemy)
{
	/*
	 * see if we can dig our way there. 
	 *
	 */


	TUniquePtr<AIDecisionTreeNode> Dig = DecisionTreeHelper::MakeLeafNode([this,Enemy]
	{
			if (!Enemy) return;

			int pos = Enemy->CanUseAblity(EAbilityType::Offensive);
		if(pos > -1)
		{
			Enemy->UseAbility(EAbilityType::Offensive, pos);
		}

	});

	return Dig;
}
