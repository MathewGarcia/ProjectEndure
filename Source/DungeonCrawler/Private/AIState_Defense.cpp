// Fill out your copyright notice in the Description page of Project Settings.

#include "AIState_Defense.h"

#include "AIEnemy.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

AIState_Defense::AIState_Defense()
{
}

void AIState_Defense::OnEnterState()
{
    AIStateBase::OnEnterState();
}

void AIState_Defense::OnExitState()
{
    AIStateBase::OnExitState();
}

void AIState_Defense::TickState(float DeltaTime)
{
    AIStateBase::TickState(DeltaTime);
}

float AIState_Defense::CalculateOptions()
{
    return AIStateBase::CalculateOptions();
}

EAIStates AIState_Defense::GetState()
{
    return EAIStates::defense;
}

void AIState_Defense::ExecuteOption(float weight, AEnemy* Enemy)
{
    AIStateBase::ExecuteOption(weight, Enemy);
}

TUniquePtr<AIDecisionTreeNode> AIState_Defense::BuildDecisionTree(AEnemy* Enemy)
{
    //first person use of words are in the perspective of the enemy.
    //feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is player stamina, 4 is player state, 5 is montage time, 6 is player facing away?
    /*                        (R)
     *           /                                   \
     * (HealthCheck)   -__________                  (InRangeOverHalfHealthAttackCheck (or dodge))
     *    /                        \_____________
     *(InRangeLessThanHalfHealthAttackCheck (or dodge))    (InRangeOverHalfHealth)
     *   /                                   \
     * (IsPlayerFacingAwayOrStunned?)
     *      /               \
     *  (MoveBackward)      (Attack)    (Should we dodge?)
     *                                      /       \
     *                                  (Dodge)     (Attack)
     */
    if (!Enemy) return nullptr;
    TUniquePtr<AIDecisionTreeNode> Root = AIStateBase::BuildDecisionTree(Enemy);
    TUniquePtr<AIDecisionTreeNode> InRangeBranch = BuildInRange(Enemy);
    TUniquePtr<AIDecisionTreeNode> NotInRange = BuildNotInRange(Enemy);
    if (Root) {
        Root->LeftChild = MoveTemp(InRangeBranch);
        Root->RightChild = MoveTemp(NotInRange);
        PrebuiltOriginal = Root.Get();
    }
    return Root;
}

TUniquePtr<AIDecisionTreeNode> AIState_Defense::BuildInRange(AEnemy* Enemy)
{
    //first person use of words are in the perspective of the enemy.
    //feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is enemy stamina, 4 is player state, 5 is montage time, 6 is player facing away?
    /*                        (R)
     *           /                                   \
     * (HealthCheck)   -__________                  (InRangeOverHalfHealthAttackCheck (or dodge))
     *    /                        \_____________
     *(InRangeLessThanHalfHealthAttackCheck (or dodge))    (InRangeOverHalfHealth)
     *   /                                   \
     * (IsPlayerFacingAwayOrStunned?)
     *      /               \
     *  (MoveBackward)      (Attack)    (Should we dodge?)
     *                                      /       \
     *                                  (Dodge)     (Attack)
     */
    TUniquePtr<AIDecisionTreeNode> ReturnNode = nullptr;
    if (!Enemy || !PlayerWeakPtr.IsValid() || !AIEnemyController) return nullptr;
    APlayerCharacter* player = PlayerWeakPtr.Get();
    if (!player) return nullptr;
    if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(player->GetPlayerState()))
    {
        //IsPlayerWeak
        TUniquePtr<AIDecisionTreeNode> PassivityCheck = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
        TUniquePtr<AIDecisionTreeNode> AttackCheck = DecisionTreeHelper::MakeDecisionNode(4, 0);
        TUniquePtr<AIDecisionTreeNode> IsPlayerFacingAway = DecisionTreeHelper::MakeDecisionNode(6, 0);
        TUniquePtr<AIDecisionTreeNode> StaminaCheck = DecisionTreeHelper::MakeDecisionNode(3, Enemy->AttackStaminaReduction);
        TUniquePtr<AIDecisionTreeNode> MontageCheck = DecisionTreeHelper::MakeDecisionNode(5, Enemy->MontageCheck);
        TUniquePtr<AIDecisionTreeNode> Dodge = DecisionTreeHelper::MakeLeafNode([Enemy]() {if (Enemy) Enemy->Dodge(FMath::RandRange(1, 3)); });
        TUniquePtr<AIDecisionTreeNode> AttackR = DecisionTreeHelper::MakeLeafNode([this]() {if (AIEnemyController) AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get()); });
        TUniquePtr<AIDecisionTreeNode> AttackL = DecisionTreeHelper::MakeLeafNode([this]() {if (AIEnemyController) AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get()); });
        TUniquePtr<AIDecisionTreeNode> MontageAttack = DecisionTreeHelper::MakeLeafNode([this]() {if (AIEnemyController) AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get()); });
        TUniquePtr<AIDecisionTreeNode> DodgeL = DecisionTreeHelper::MakeLeafNode([Enemy]() {if (Enemy) Enemy->Dodge(FMath::RandRange(1, 3)); });
        TUniquePtr<AIDecisionTreeNode> MoveBackward = DecisionTreeHelper::MakeLeafNode([this, Enemy]() {
            if (!PlayerWeakPtr.IsValid() || !AIEnemyController) return;
            APlayerCharacter* player = PlayerWeakPtr.Get();
            if (!player) return;
            if (Enemy->bUseControllerRotationYaw)
            {
                Enemy->bUseControllerRotationYaw = false;
            }
            // Compute direction to player (ignoring Z to keep movement parallel to ground)
            FVector ToPlayer = (player->GetActorLocation() - Enemy->GetActorLocation());
            ToPlayer.Z = 0;
            if (!ToPlayer.IsNearlyZero()) ToPlayer.Normalize();
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
                /*UE_LOG(LogTemp, Warning, TEXT("Move Target: %s"), *TargetLocation.ToString());
                UE_LOG(LogTemp, Warning, TEXT("Enemy Location: %s"), *Enemy->GetActorLocation().ToString());
                UE_LOG(LogTemp, Warning, TEXT("Move Result: %d"), result);*/
            }
        });
        TUniquePtr<AIDecisionTreeNode> AttackOverHalfHealth = DecisionTreeHelper::MakeLeafNode([this]() {if (AIEnemyController) AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get()); });
        StaminaCheck->LeftChild = MoveTemp(Dodge);
        StaminaCheck->RightChild = MoveTemp(AttackR);
        MontageCheck->RightChild = MoveTemp(StaminaCheck);
        MontageCheck->LeftChild = MoveTemp(MontageAttack);
        IsPlayerFacingAway->LeftChild = MoveTemp(DodgeL);
        IsPlayerFacingAway->RightChild = MoveTemp(AttackL);
        AttackCheck->LeftChild = MoveTemp(IsPlayerFacingAway);
        AttackCheck->RightChild = MoveTemp(MontageCheck);
        PassivityCheck->RightChild = MoveTemp(AttackOverHalfHealth);
        PassivityCheck->LeftChild = MoveTemp(AttackCheck);
        ReturnNode = MoveTemp(PassivityCheck);
    }
    return ReturnNode;
}

TUniquePtr<AIDecisionTreeNode> AIState_Defense::BuildNotInRange(AEnemy* Enemy)
{
    /*
     *      (Check player Health)
     *          /           \
     *      (IsPlayerFacing Away)
     *      /  \           /   \
     *
     */
    TUniquePtr<AIDecisionTreeNode> ReturnNode = nullptr;
    if (!Enemy || !PlayerWeakPtr.IsValid() || !AIEnemyController) return nullptr;
    APlayerCharacter* player = PlayerWeakPtr.Get();
    if (!player) return nullptr;
    if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(player->GetPlayerState()))
    {
        //Check Player's health
        TUniquePtr<AIDecisionTreeNode> Attack = DecisionTreeHelper::MakeLeafNode([this]() {if (AIEnemyController) AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get()); });
        ReturnNode = MoveTemp(Attack);
    }
    return ReturnNode;
}

TUniquePtr<AIDecisionTreeNode> AIState_Defense::BuildBlock(AEnemy* Enemy)
{
    TUniquePtr<AIDecisionTreeNode> Root = DecisionTreeHelper::MakeLeafNode([Enemy]() {if (Enemy) Enemy->Dodge(FMath::RandRange(1, 3)); });
    PrebuiltBlockOrDodge = Root.Get();
    return Root;
}

