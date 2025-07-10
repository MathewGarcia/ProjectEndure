#include "AIState_Attack_Boss_Necro.h"
#include "Enemy.h"
#include "AIEnemy.h"
#include "PlayerCharacter.h"

TUniquePtr<AIDecisionTreeNode> AIState_Attack_Boss_Necro::BuildInRange(AEnemy* Enemy)
{

	if (!PlayerWeakPtr.IsValid()) return nullptr;


	/*
	 *check if we have stamina to attack.
	 *for the most part, we want to get away from the player if we can. Maybe actively move backward while trying to see if we can attack?
	 */

	 //feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is enemy stamina, 4 is player state, 5 is montage time, 6 is player facing away?, 7 used for checking if AI should buff or go to near by enemies., 8 Passivity float


	TUniquePtr<AIDecisionTreeNode> StaminaCheck = DecisionTreeHelper::MakeDecisionNode(3, Enemy->AttackStaminaReduction);
	TUniquePtr<AIDecisionTreeNode> PassivityCheck = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
	TUniquePtr<AIDecisionTreeNode> MoveBackward = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{

			if (!PlayerWeakPtr.IsValid()) return;

			APlayerCharacter* player = PlayerWeakPtr.Get();
			UE_LOG(LogTemp, Warning, TEXT("NECRO MOVE TO BACKWARD CALLED IN RANGE"));

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
			float MoveDistance = 500.f;
			FVector TargetLocation = Enemy->GetActorLocation() - (ToPlayer * MoveDistance) + (PerpendicularOffset * SideStepAmount);

			// Ensure AI doesn’t keep recalculating movement every frame
			if (!AIEnemyController->IsFollowingAPath())
			{
				EPathFollowingRequestResult::Type result = AIEnemyController->MoveToLocation(TargetLocation);
			}

		});

	TUniquePtr<AIDecisionTreeNode> Wait = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{
			if (!PlayerWeakPtr.IsValid()) return;

			if (APlayerCharacter* player = PlayerWeakPtr.Get())
			{
				UE_LOG(LogTemp, Warning, TEXT("NECRO MOVE TO LOCATION CALLED IN RANGE"));
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
			UE_LOG(LogTemp, Warning, TEXT("NECRO ATTACK IN RANGE"));

			Enemy->Attack();
		});

	PassivityCheck->RightChild = MoveTemp(Attack);
	PassivityCheck->LeftChild = MoveTemp(Wait);
	StaminaCheck->RightChild = MoveTemp(PassivityCheck);
	StaminaCheck->LeftChild = MoveTemp(MoveBackward);

	return StaminaCheck;
}

TUniquePtr<AIDecisionTreeNode> AIState_Attack_Boss_Necro::BuildNotInRange(AEnemy* Enemy)
{
	//feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is enemy stamina, 4 is player state, 5 is montage time, 6 is player facing away?, 7 used for checking if AI should buff or go to near by enemies., 8 Passivity float
	//if we are not in range, we want to still stay a minimum distance away from the player. However, we should summon enemies.

	if (!PlayerWeakPtr.IsValid()) return nullptr;


	TUniquePtr<AIDecisionTreeNode> DistanceCheck = DecisionTreeHelper::MakeDecisionNode(0, 800.f);
	TUniquePtr<AIDecisionTreeNode> MoveBackward = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{

			UE_LOG(LogTemp, Warning, TEXT("NECRO MOVEBACKWARD CALLED NIR"));

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
			float SideStepAmount = FMath::FRandRange(-50.f, 50.f);  // Random left or right shift

			// Compute target location
			float MoveDistance = 800.f;
			FVector TargetLocation = Enemy->GetActorLocation() - (ToPlayer * MoveDistance) + (PerpendicularOffset * SideStepAmount);

			// Ensure AI doesn’t keep recalculating movement every frame
			if (!AIEnemyController->IsFollowingAPath())
			{
				EPathFollowingRequestResult::Type result = AIEnemyController->MoveToLocation(TargetLocation);
			}

		});


	TUniquePtr<AIDecisionTreeNode> Attack = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
		{
			UE_LOG(LogTemp, Warning, TEXT("NECRO ATTACK CALLED NIR"));

			int pos = Enemy->CanUseAblity(EAbilityType::Offensive);

			if (pos > -1)
			{
				Enemy->UseAbility(EAbilityType::Offensive, pos);
			}
			else
			{
				//movebackward
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
				float SideStepAmount = FMath::FRandRange(-50.f, 50.f);  // Random left or right shift

				// Compute target location
				float MoveDistance = 500.f;
				FVector TargetLocation = Enemy->GetActorLocation() - (ToPlayer * MoveDistance) + (PerpendicularOffset * SideStepAmount);

				// Ensure AI doesn’t keep recalculating movement every frame
				if (!AIEnemyController->IsFollowingAPath())
				{
					EPathFollowingRequestResult::Type result = AIEnemyController->MoveToLocation(TargetLocation);
				}
			}
		});

	DistanceCheck->RightChild = MoveTemp(Attack);
	DistanceCheck->LeftChild = MoveTemp(MoveBackward);

	return DistanceCheck;
}

TUniquePtr<AIDecisionTreeNode> AIState_Attack_Boss_Necro::BuildDecisionTree(AEnemy* Enemy)
{

	TUniquePtr<AIDecisionTreeNode> Root = AIState_Attack::BuildDecisionTree(Enemy);
	TUniquePtr<AIDecisionTreeNode> InRange = BuildInRange(Enemy);
	TUniquePtr<AIDecisionTreeNode> NotInRange = BuildNotInRange(Enemy);
	Root->LeftChild = MoveTemp(InRange);
	Root->RightChild = MoveTemp(NotInRange);

	return Root;
}

void AIState_Attack_Boss_Necro::TickState(float DeltaTime)
{
	AIStateBase::TickState(DeltaTime);
}
