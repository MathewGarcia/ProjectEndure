#include "AIState_Attack_Mostrosity.h"
#include "PlayerCharacter.h"
#include "AIEnemy.h"

TUniquePtr<AIDecisionTreeNode> AIState_Attack_Mostrosity::BuildInRange(AEnemy* Enemy)
{
	//feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is enemy stamina, 4 is player state, 5 is montage time, 6 is player facing away?, 7 used for checking if AI should buff or go to near by enemies., 8 Passivity float

	/*
	 *				(StamCheck)
	 *				/			\
	*			(Strafe)	(Passivity Check)
	 *							/			\
	 *						(Strafe)	(Attack)
	 *
	 */

	if (!PlayerWeakPtr.IsValid() || !Enemy || !AIEnemyController) return nullptr;

	TUniquePtr<AIDecisionTreeNode> PassivityCheck = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
	TUniquePtr<AIDecisionTreeNode> StaminaCheck = DecisionTreeHelper::MakeDecisionNode(3, Enemy->AttackStaminaReduction);
	TUniquePtr<AIDecisionTreeNode> StrafePCL = DecisionTreeHelper::MakeLeafNode([this,Enemy]()
		{
			if (!PlayerWeakPtr.IsValid() || !AIEnemyController) return;

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

	TUniquePtr<AIDecisionTreeNode> StrafeSCL = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{
			if (!PlayerWeakPtr.IsValid() || !AIEnemyController) return;

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

	TUniquePtr<AIDecisionTreeNode> Attack = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
	{
			if (!Enemy) return;
			Enemy->Attack();
	});

	PassivityCheck->LeftChild = MoveTemp(StrafePCL);
	PassivityCheck->RightChild = MoveTemp(Attack);

	StaminaCheck->RightChild = MoveTemp(PassivityCheck);
	StaminaCheck->LeftChild = MoveTemp(StrafeSCL);

	return StaminaCheck;

}

TUniquePtr<AIDecisionTreeNode> AIState_Attack_Mostrosity::BuildNotInRange(AEnemy* Enemy)
{
	/*
	 *	passivity check
	 *		/		\
	 *	strafe			Stamina check
	 *					/		\
	 *				strafe		hook shot
	 *
	 */

	if (!PlayerWeakPtr.IsValid() || !Enemy || !AIEnemyController) return nullptr;

	TUniquePtr<AIDecisionTreeNode> PassivityCheck = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
	TUniquePtr<AIDecisionTreeNode> StaminaCheck = DecisionTreeHelper::MakeDecisionNode(3, Enemy->AttackStaminaReduction);
	TUniquePtr<AIDecisionTreeNode> StrafePCL = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{
			if (!PlayerWeakPtr.IsValid() || !AIEnemyController) return;

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

	TUniquePtr<AIDecisionTreeNode> StrafeSCL = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{
			if (!PlayerWeakPtr.IsValid() || !AIEnemyController) return;

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

	TUniquePtr<AIDecisionTreeNode> HookShot = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{
			if (!Enemy) return;
			int pos = Enemy->CanUseAblity(EAbilityType::Offensive);
			if(pos > -1)
			{
				Enemy->UseAbility(EAbilityType::Offensive, pos);
			}

			UE_LOG(LogTemp, Error, TEXT("Should be using hook shot"));
		});

	StaminaCheck->RightChild = MoveTemp(HookShot);
	StaminaCheck->LeftChild = MoveTemp(StrafeSCL);
	/*PassivityCheck->RightChild = MoveTemp(StaminaCheck);
	PassivityCheck->LeftChild = MoveTemp(StrafePCL);*/
	return StaminaCheck;
}

TUniquePtr<AIDecisionTreeNode> AIState_Attack_Mostrosity::BuildDecisionTree(AEnemy* Enemy)
{
	TUniquePtr<AIDecisionTreeNode> Root = AIStateBase::BuildDecisionTree(Enemy);

	TUniquePtr<AIDecisionTreeNode> InRange = BuildInRange(Enemy);
	TUniquePtr<AIDecisionTreeNode> NotInRange = BuildNotInRange(Enemy);

	if (Root) {
		Root->LeftChild = MoveTemp(InRange);
		Root->RightChild = MoveTemp(NotInRange);
	}

	return Root;
}
