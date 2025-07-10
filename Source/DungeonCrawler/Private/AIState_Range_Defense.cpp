#include "AIState_Range_Defense.h"
#include "AIEnemy.h"
#include "PlayerCharacter.h"
#include "Enemy.h"

TUniquePtr<AIDecisionTreeNode> AIState_Range_Defense::BuildInRange(AEnemy* Enemy)
{

	//feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is enemy stamina, 4 is player state, 5 is montage time, 6 is player facing away?, 7 used for checking if AI should buff or go to near by enemies., 8 Passivity float

	/* 
	 *     (Player Attacked)
	 *		/				\
	 *	(StaminaCheckL)		 (Attack)
	 *		/			\
	 *	(MoveBackwardL)  (Dodge)
	 */
	TUniquePtr<AIDecisionTreeNode> PassivityCheck = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
	TUniquePtr<AIDecisionTreeNode> StaminaCheckL = DecisionTreeHelper::MakeDecisionNode(3, Enemy->DodgeStaminaReduction);
	TUniquePtr<AIDecisionTreeNode> MoveBackwardL = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{
			if (!PlayerWeakPtr.IsValid()) return;

			APlayerCharacter* player = PlayerWeakPtr.Get();

			UE_LOG(LogTemp, Warning, TEXT("MAGE MOVING BACKWARD"));
			if (Enemy->bUseControllerRotationYaw)
			{
				Enemy->bUseControllerRotationYaw = false;
			}

			// Compute direction to player (ignoring Z to keep movement parallel to ground)
			FVector ToPlayer = (player->GetActorLocation() - Enemy->GetActorLocation());
			ToPlayer.Z = 0;
			ToPlayer.Normalize();

			// Make the AI face the player
			FRotator FacePlayerRotation = ToPlayer.Rotation();
			Enemy->SetActorRotation(FacePlayerRotation);

			// Compute a perpendicular vector for sidestepping (left/right offset)
			FVector PerpendicularOffset = FVector(-ToPlayer.Y, ToPlayer.X, 0).GetSafeNormal();  // Rotate 90 degrees in 2D space
			float SideStepAmount = FMath::FRandRange(-100.f, 100.f);  // Random left or right shift

			// Compute target location
			float MoveDistance = 100.f;
			FVector TargetLocation = Enemy->GetActorLocation() - (ToPlayer * MoveDistance) + (PerpendicularOffset * SideStepAmount);

			// Debugging visualization
			/*
			DrawDebugLine(world, Enemy->GetActorLocation(), TargetLocation, FColor::Red, true);
			*/

			// Ensure AI doesn’t keep recalculating movement every frame
			if (!AIEnemyController->IsFollowingAPath())
			{
				EPathFollowingRequestResult::Type result = AIEnemyController->MoveToLocation(TargetLocation);
				UE_LOG(LogTemp, Warning, TEXT("Move Target: %s"), *TargetLocation.ToString());
				UE_LOG(LogTemp, Warning, TEXT("Enemy Location: %s"), *Enemy->GetActorLocation().ToString());
				UE_LOG(LogTemp, Warning, TEXT("Move Result: %d"), result);
			}

		});

	TUniquePtr<AIDecisionTreeNode> DodgeL = DecisionTreeHelper::MakeLeafNode([Enemy]() {Enemy->Dodge(FMath::RandRange(1, 3)); });

	StaminaCheckL->LeftChild = MoveTemp(MoveBackwardL);
	StaminaCheckL->RightChild = MoveTemp(DodgeL);
	PassivityCheck->LeftChild = MoveTemp(StaminaCheckL);

	TUniquePtr<AIDecisionTreeNode> SetToAttack = DecisionTreeHelper::MakeLeafNode([this]() {AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get()); });
	PassivityCheck->RightChild = MoveTemp(SetToAttack);

	return PassivityCheck;
}	

TUniquePtr<AIDecisionTreeNode> AIState_Range_Defense::BuildNotInRange(AEnemy* Enemy)
{
	TUniquePtr<AIDecisionTreeNode> SetToAttack = DecisionTreeHelper::MakeLeafNode([this]() {AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get()); });

	return SetToAttack;
}

TUniquePtr<AIDecisionTreeNode> AIState_Range_Defense::BuildDecisionTree(AEnemy* Enemy)
{
	TUniquePtr<AIDecisionTreeNode> Root = AIStateBase::BuildDecisionTree(Enemy);

	Root->LeftChild = BuildInRange(Enemy);
	Root->RightChild = BuildNotInRange(Enemy);

	return Root;
}
