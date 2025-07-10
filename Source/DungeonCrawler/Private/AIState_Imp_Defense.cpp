#include "AIState_Imp_Defense.h"

#include "Enemy.h"
#include "AIEnemy.h"
#include "PlayerCharacter.h"



EAIStates AIState_Imp_Defense::GetState()
{
	return AIState_Defense::GetState();
}

TUniquePtr<AIDecisionTreeNode> AIState_Imp_Defense::BuildDecisionTree(AEnemy* Enemy)
{
	/*											(R)
	 *										/				\
	 *									(Should Buff)		(Should buff)
	 *										/0     \1			/0					\1
	 *									(InRange)  (BuffLogic)	(NotinRange)		(buff logic)
	 */	

	TUniquePtr<AIDecisionTreeNode> Root = AIStateBase::BuildDecisionTree(Enemy);
	TUniquePtr<AIDecisionTreeNode> InRange = AIState_Defense::BuildInRange(Enemy);
	TUniquePtr<AIDecisionTreeNode> NotInRange = AIState_Defense::BuildNotInRange(Enemy);
	TUniquePtr<AIDecisionTreeNode> ShouldBuffAllies_L = BuildBuffAllies(Enemy);
	TUniquePtr<AIDecisionTreeNode> ShouldBuffAllies_R = BuildBuffAllies(Enemy);

	ShouldBuffAllies_L->LeftChild = MoveTemp(InRange);
	ShouldBuffAllies_R->LeftChild = MoveTemp(NotInRange);

	Root->LeftChild = MoveTemp(ShouldBuffAllies_L);
	Root->RightChild = MoveTemp(ShouldBuffAllies_R);

	return Root;

}

TUniquePtr<AIDecisionTreeNode> AIState_Imp_Defense::BuildBuffAllies(AEnemy* Enemy)
{
	//feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is enemy stamina, 4 is player state, 5 is montage time, 6 is player facing away?, 7 can the ai buff?

	TUniquePtr<AIDecisionTreeNode> ShouldBuffAllies = DecisionTreeHelper::MakeDecisionNode(7, 0.5);
	TUniquePtr<AIDecisionTreeNode> BuffAlliesStamCheck = DecisionTreeHelper::MakeDecisionNode(3, 30);
	TUniquePtr<AIDecisionTreeNode> BuffAllies = DecisionTreeHelper::MakeLeafNode([Enemy]() {Enemy->BuffFriendlies(); });
	TUniquePtr<AIDecisionTreeNode> MoveBack = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{

			if (!PlayerWeakPtr.IsValid()) return;

			APlayerCharacter* player = PlayerWeakPtr.Get();

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
			float MoveDistance = 1000.f;
			FVector TargetLocation = Enemy->GetActorLocation() - (ToPlayer * MoveDistance) + (PerpendicularOffset * SideStepAmount);

			// Debugging visualization
			/*
			DrawDebugLine(world, Enemy->GetActorLocation(), TargetLocation, FColor::Red, true);
			*/

			// Ensure AI doesn’t keep recalculating movement every frame
			if (!AIEnemyController->IsFollowingAPath())
			{
				EPathFollowingRequestResult::Type result = AIEnemyController->MoveToLocation(TargetLocation);
			}

		});

	BuffAlliesStamCheck->RightChild = MoveTemp(BuffAllies);
	BuffAlliesStamCheck->LeftChild = MoveTemp(MoveBack);
	ShouldBuffAllies->RightChild = MoveTemp(BuffAlliesStamCheck);


	return ShouldBuffAllies;
}
