#include "AIState_Range_Attack_Base.h"
#include "PlayerCharacter.h"
#include "AIEnemy.h"


void AIState_Range_Attack_Base::TickState(float DeltaTime)
{
	AIState_Attack::TickState(DeltaTime);
}

TUniquePtr<AIDecisionTreeNode> AIState_Range_Attack_Base::BuildInRange(AEnemy* Enemy)
{
    /*
     *              (AggressionCheck)
     *              /               \
     *           (Wait)          (StaminaCheckR)
     *                                 /        \ 
     *                             (Defense)     (Attack)
     */
    if (!Enemy || !AIEnemyController) return nullptr;
    TUniquePtr<AIDecisionTreeNode> StaminaCheckR = DecisionTreeHelper::MakeDecisionNode(3, Enemy->AttackStaminaReduction);
    TUniquePtr<AIDecisionTreeNode> AggressionCheck = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
    TUniquePtr<AIDecisionTreeNode> Wait = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
        {
            if (!PlayerWeakPtr.IsValid() || !AIEnemyController) return;
            if (APlayerCharacter* player = PlayerWeakPtr.Get())
            {
                UE_LOG(LogTemp, Warning, TEXT("MAGE MOVE TO LOCATION CALLED IN RANGE"));
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
    TUniquePtr<AIDecisionTreeNode> Attack = DecisionTreeHelper::MakeLeafNode([Enemy]() {if (Enemy) Enemy->Attack(); });
    TUniquePtr<AIDecisionTreeNode> Defense = DecisionTreeHelper::MakeLeafNode([this]() {if (AIEnemyController) AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get()); });
    TUniquePtr<AIDecisionTreeNode> Defense2 = DecisionTreeHelper::MakeLeafNode([this]() {if (AIEnemyController) AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get()); });
    AggressionCheck->LeftChild = MoveTemp(Wait);
    StaminaCheckR->RightChild = MoveTemp(Attack);
    StaminaCheckR->LeftChild = MoveTemp(Defense2);
    AggressionCheck->RightChild = MoveTemp(StaminaCheckR);
    return AggressionCheck;
}

TUniquePtr<AIDecisionTreeNode> AIState_Range_Attack_Base::BuildNotInRange(AEnemy* Enemy)
{

    /*    (AggressionCheck)
     *        /          \
     *      (Wait)  (Stamina Check)
     *             /        \
     *          (Defense)   (Attack)
     */
    if (!Enemy || !AIEnemyController) return nullptr;
    TUniquePtr<AIDecisionTreeNode> AggressionCheck = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
    TUniquePtr<AIDecisionTreeNode> StaminaCheck = DecisionTreeHelper::MakeDecisionNode(3, Enemy->AttackStaminaReduction);
    TUniquePtr<AIDecisionTreeNode> Attack = DecisionTreeHelper::MakeLeafNode([Enemy]() {if (Enemy) Enemy->Attack(); });
    TUniquePtr<AIDecisionTreeNode> Defense = DecisionTreeHelper::MakeLeafNode([this]() {if (AIEnemyController) AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get()); });
    TUniquePtr<AIDecisionTreeNode> Wait = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
        {
            if (!PlayerWeakPtr.IsValid() || !AIEnemyController) return;
            if (APlayerCharacter* player = PlayerWeakPtr.Get())
            {
                UE_LOG(LogTemp, Warning, TEXT("MAGE WAIT"));
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
    StaminaCheck->RightChild = MoveTemp(Attack);
    StaminaCheck->LeftChild = MoveTemp(Defense);
    AggressionCheck->RightChild = MoveTemp(StaminaCheck);
    AggressionCheck->LeftChild = MoveTemp(Wait);
    return AggressionCheck;
}

TUniquePtr<AIDecisionTreeNode> AIState_Range_Attack_Base::BuildDecisionTree(AEnemy* Enemy)
{
    if (!Enemy) return nullptr;
    TUniquePtr<AIDecisionTreeNode> Root = AIStateBase::BuildDecisionTree(Enemy);
    if (!Root) return nullptr;
    Root->LeftChild = BuildInRange(Enemy);
    Root->RightChild = BuildNotInRange(Enemy);
    return Root;
}
