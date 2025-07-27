// Fill out your copyright notice in the Description page of Project Settings.

#include "AIState_Attack.h"

#include "AIEnemy.h"
#include "Enemy.h"
#include "MainGameInstance.h"
#include "Navigation\PathFollowingComponent.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void AIState_Attack::OnEnterState()
{
    AIStateBase::OnEnterState();
}

void AIState_Attack::OnExitState()
{
    AIStateBase::OnExitState();
}

void AIState_Attack::TickState(float DeltaTime)
{
    AIStateBase::TickState(DeltaTime);
    // Nullptr safety for player and controller
    if (!PlayerWeakPtr.IsValid() || !AIEnemyController) return;
    APlayerCharacter* player = PlayerWeakPtr.Get();
    if (!player) return;
    if (AEnemy* Enemy = Cast<AEnemy>(AIEnemyController->GetPawn())) {
        if (world)
        {
            if (Enemy->CanAttack() && !AIEnemyController->IsFollowingAPath() && PlayerDistance > Enemy->AttackDistance*20.f) {
                if (!Enemy->bUseControllerRotationYaw)
                {
                    Enemy->bUseControllerRotationYaw = true;
                }
                EPathFollowingRequestResult::Type Result = AIEnemyController->MoveToActor(player, Enemy->AttackDistance);
                if (Result == 1)
                {
                    IsInRange = true;
                }
                else if (Result != 1 && IsInRange)
                {
                    IsInRange = false;
                }
                UE_LOG(LogTemp, Warning, TEXT("RESULT: %d"), Result);
            }
        }
    }
}

float AIState_Attack::CalculateOptions()
{
    return AIStateBase::CalculateOptions();
}

EAIStates AIState_Attack::GetState()
{
    return EAIStates::attack;
}

TUniquePtr<AIDecisionTreeNode> AIState_Attack::BuildDecisionTree(AEnemy* Enemy)
{
    //feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is player stamina, 4 is player state, 5 is montage time, 6 is player facing away?
    /*
     *                                                        (R)
     *                                   /                                      \
     *                (InRangeIsPlayerWeak)   _______________                  (NotInRangeIsEnemyWeak)
     *                  /                                     \
     *                 /                                       \
     *              (PlayerWeakIsEnemyWeak)                      ---------------------------(PlayerStrongIsEnemyWeak)-----------
     *                  /                   \                                              /                                     \
     *                 /                     \                                            /                                     (PlayerStrongEnemyStrongAttack)
     *     (PlayerWeakEnemyWeakStaminaCheck)   (PlayerStrongEnemyStrongCheckStamina)     (EnemyWeakShouldWeAttack?)
     *                                              /                   \                      /                \
     *                           (PlayerStrongEnemyStrongNoStamina)      (CanAttack)        (AttackOrDodge?)       (AttackOrDodge2)
     *                                                                                          /     \                   /     \
     *                                                                                        (Defense) (Attack)       (Defense)   (Attack)
     */
    if (!Enemy) return nullptr;
    TUniquePtr<AIDecisionTreeNode> Root = AIStateBase::BuildDecisionTree(Enemy);
    if (!Root) return nullptr;
    Root->LeftChild = BuildInRange(Enemy);
    Root->RightChild = BuildNotInRange(Enemy);
    return Root;
}

TUniquePtr<AIDecisionTreeNode> AIState_Attack::BuildInRange(AEnemy* Enemy)
{
    //feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is enemy stamina, 4 is player state, 5 is montage time, 6 is player facing away?
    /*
    *                                                        (R)
    *                                   /                                      \
    *                (InRangeIsPlayerWeak)   _______________                    (Super Aggressive) //not implemented yet.
    *                  /                                     \
    *                 /                                       \
    *              (PlayerWeakIsEnemyWeak)                      ---------------------------(PlayerStrongIsEnemyWeak)-----------
    *                  /                   \                                              /                                     \
    *                 /                     \                                            /                                     (PlayerStrongEnemyStrongAttack)
    *     (PlayerWeakEnemyWeakStaminaCheck)   (PlayerStrongEnemyStrongCheckStamina)     (EnemyWeakShouldWeAttack?)
    *                                              /                   \                      /                \
    *                                            (Defense)            (CanAttack)        (AttackOrDodge?)       (AttackOrDodge2)
    *                                                                                            /     \                   /     \
    *                                                                                        (Defense) (Attack)       (Defense)   (Attack)
    */
    TUniquePtr<AIDecisionTreeNode> InRange = nullptr;
    if (!Enemy || !PlayerWeakPtr.IsValid()) return nullptr;
    APlayerCharacter* player = PlayerWeakPtr.Get();
    if (!player) return nullptr;
    if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(player->GetPlayerState()))
    {

        TUniquePtr<AIDecisionTreeNode>ShouldWait = DecisionTreeHelper::MakeDecisionNode(10, 0.5);

        //IsPlayerWeak
        TUniquePtr<AIDecisionTreeNode> IsPlayerWeak = DecisionTreeHelper::MakeDecisionNode(1, PCS->GetTotalHealth() / 2);
        TUniquePtr<AIDecisionTreeNode> IsEnemyWeak = DecisionTreeHelper::MakeDecisionNode(2, Enemy->MaxHealth / 2);
        TUniquePtr<AIDecisionTreeNode> StaminaCheckL = DecisionTreeHelper::MakeDecisionNode(3, Enemy->AttackStaminaReduction);
        TUniquePtr<AIDecisionTreeNode> StaminaCheckR = DecisionTreeHelper::MakeDecisionNode(3, Enemy->AttackStaminaReduction);
        TUniquePtr<AIDecisionTreeNode> ShouldAttackL = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
        TUniquePtr<AIDecisionTreeNode> CanAttackL = DecisionTreeHelper::MakeLeafNode([this,Enemy]() {Enemy->Attack(); UE_LOG(LogTemp, Warning, TEXT("ATTACK CALLED FROM CANATTACKL WITH : %f"), AIEnemyController->features[8]); });
        TUniquePtr<AIDecisionTreeNode> DefenseL = DecisionTreeHelper::MakeLeafNode([this,Enemy]() {
        {
            if (!Enemy->CanAttack()) return;
        }
        AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get());
            });

        TUniquePtr<AIDecisionTreeNode> DefenseFromLackOfPassivity = DecisionTreeHelper::MakeLeafNode([this,Enemy]() {
        {
            if (!Enemy->CanAttack()) return;
        }
        AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get());
            });

        TUniquePtr<AIDecisionTreeNode> ShouldAttackRL = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
        TUniquePtr<AIDecisionTreeNode> ShouldAttackRR = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
        TUniquePtr<AIDecisionTreeNode> CanAttackRR = DecisionTreeHelper::MakeLeafNode([Enemy]() {Enemy->Attack(); });
        TUniquePtr<AIDecisionTreeNode> DefenseRR = DecisionTreeHelper::MakeLeafNode([this]() {AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get()); });
        TUniquePtr<AIDecisionTreeNode> CanAttackRL = DecisionTreeHelper::MakeLeafNode([this,Enemy]() {Enemy->Attack(); UE_LOG(LogTemp, Warning, TEXT("ATTACK CALLED FROM CANATTACKRL WITH : %f"), AIEnemyController->features[8]); });
        TUniquePtr<AIDecisionTreeNode> DefenseRL = DecisionTreeHelper::MakeLeafNode([this]() {AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get()); });

        //Player Is Strong

        //strong is enemy weak can probably be replaced with an Passivity check??
        TUniquePtr<AIDecisionTreeNode> StrongIsEnemyWeak = DecisionTreeHelper::MakeDecisionNode(2, Enemy->MaxHealth / 2);
        TUniquePtr<AIDecisionTreeNode> EnemyStrongAttack = DecisionTreeHelper::MakeLeafNode([this,Enemy]() {Enemy->Attack(); UE_LOG(LogTemp, Warning, TEXT("ATTACK CALLED FROM ENEMYSTRONGATTACK WITH : %f"), AIEnemyController->features[8]); });
        TUniquePtr<AIDecisionTreeNode> PassivityCheck = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
        TUniquePtr<AIDecisionTreeNode> PassivityCheckL = DecisionTreeHelper::MakeLeafNode([this]() {AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get()); });

        TUniquePtr<AIDecisionTreeNode> AttackCheckL = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
        TUniquePtr<AIDecisionTreeNode> DefenseStrongL = DecisionTreeHelper::MakeLeafNode([this]() {AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get()); });
        TUniquePtr<AIDecisionTreeNode> AttackL = DecisionTreeHelper::MakeLeafNode([this,Enemy]() {Enemy->Attack(); UE_LOG(LogTemp, Warning, TEXT("ATTACK CALLED FROM ATTACKL WITH : %f"), AIEnemyController->features[8]); });
        TUniquePtr<AIDecisionTreeNode> AttackCheckR = DecisionTreeHelper::MakeDecisionNode(8, Enemy->Passivity);
        TUniquePtr<AIDecisionTreeNode> DefenseStrongR = DecisionTreeHelper::MakeLeafNode([this]() {AIEnemyController->ChangeAIState(AIEnemyController->DefenseState.Get()); });
        TUniquePtr<AIDecisionTreeNode> AttackR = DecisionTreeHelper::MakeLeafNode([this,Enemy]() {Enemy->Attack(); UE_LOG(LogTemp, Warning, TEXT("ATTACK CALLED FROM ATTACKR WITH : %f"), AIEnemyController->features[8]); });
        TUniquePtr<AIDecisionTreeNode> OpportunityCheck = DecisionTreeHelper::MakeDecisionNode(5, 0.5);
        TUniquePtr<AIDecisionTreeNode> Wait = DecisionTreeHelper::MakeLeafNode([this, Enemy]()
        {
                if (!PlayerWeakPtr.IsValid()) return;

                if(APlayerCharacter* player = PlayerWeakPtr.Get())
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

        //Left
        IsPlayerWeak->LeftChild = MoveTemp(IsEnemyWeak);
        IsPlayerWeak->LeftChild->LeftChild = MoveTemp(StaminaCheckL);
        IsPlayerWeak->LeftChild->LeftChild->RightChild = MoveTemp(ShouldAttackL);
        IsPlayerWeak->LeftChild->LeftChild->RightChild->RightChild = MoveTemp(CanAttackL);
        IsPlayerWeak->LeftChild->LeftChild->RightChild->LeftChild = MoveTemp(DefenseFromLackOfPassivity);
        IsPlayerWeak->LeftChild->LeftChild->LeftChild = MoveTemp(DefenseL);

        //Right
        ShouldAttackRL->LeftChild = MoveTemp(DefenseRL);
        ShouldAttackRL->RightChild = MoveTemp(CanAttackRL);
        ShouldAttackRR->RightChild = MoveTemp(CanAttackRR);
        ShouldAttackRR->LeftChild = MoveTemp(DefenseRR);
        StaminaCheckR->LeftChild = MoveTemp(ShouldAttackRL);
        StaminaCheckR->RightChild = MoveTemp(ShouldAttackRR);
        

        //Player Is Strong
        AttackCheckL->LeftChild = MoveTemp(DefenseStrongL);
        AttackCheckL->RightChild = MoveTemp(AttackL);

        AttackCheckR->LeftChild = MoveTemp(DefenseStrongR);
        AttackCheckR->RightChild = MoveTemp(AttackR);

        OpportunityCheck->LeftChild = MoveTemp(AttackCheckL);
        OpportunityCheck->RightChild = MoveTemp(AttackCheckR);

        //PassivityCheckBranch
        PassivityCheck->RightChild = MoveTemp(EnemyStrongAttack);
        PassivityCheck->LeftChild = MoveTemp(PassivityCheckL);
        StrongIsEnemyWeak->LeftChild = MoveTemp(OpportunityCheck);
        StrongIsEnemyWeak->RightChild = MoveTemp(PassivityCheck);


        //InRange root
        InRange = MoveTemp(ShouldWait);
        InRange->LeftChild = MoveTemp(StrongIsEnemyWeak);
        InRange->LeftChild->RightChild = MoveTemp(StaminaCheckR);
        InRange->RightChild = MoveTemp(Wait);
        return InRange;
    }
    return nullptr;
}

TUniquePtr<AIDecisionTreeNode> AIState_Attack::BuildNotInRange(AEnemy* Enemy)
{
    /*
     *      (PlayerHealthCheck)
     *          /       \
     *        (CheckOurHealth)
     *         /  \    /   \
     *         (OpportunityAttack)
     *         /            \
     *        (Defense)      (Attack)
     *
     */
    if (!Enemy) return nullptr;
    TUniquePtr<AIDecisionTreeNode> DoesHaveLongRange = DecisionTreeHelper::MakeDecisionNode(9, 0.5);
    TUniquePtr<AIDecisionTreeNode> WeHaveLongRange = DecisionTreeHelper::MakeLeafNode([Enemy]() {
        if (!Enemy) return;
        UE_LOG(LogTemp, Warning, TEXT("Throwing rock"));
        int32 pos = Enemy->CanUseAblity(EAbilityType::Offensive);
        if(pos > -1)
        {
            Enemy->UseAbility(EAbilityType::Offensive, pos);
        }
    });
    DoesHaveLongRange->RightChild = MoveTemp(WeHaveLongRange);
    return DoesHaveLongRange;
}

void AIState_Attack::ExecuteOption(float weight,AEnemy*Enemy)
{
    AIStateBase::ExecuteOption(weight,Enemy);
}

