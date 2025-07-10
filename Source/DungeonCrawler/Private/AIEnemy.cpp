// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy.h"

#include "AIDecisionTreeNode.h"
#include "AIState_Attack.h"
#include "AIState_Attack_Boss_Necro.h"
#include "AIState_Attack_Mostrosity.h"
#include "AIState_Idle.h"
#include "AIState_Imp_Defense.h"
#include "AIState_Range_Attack_Base.h"
#include "AIState_Range_Defense.h"
#include "AIState_Salamander_Attack.h"
#include "Enemy.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

AAIEnemy::AAIEnemy()
{
}

void AAIEnemy::ChangeAIState(AIStateBase* AIState)
{

	if(CurrentState)
	{
		CurrentState->OnExitState();
	}
	CurrentState = AIState;
	UE_LOG(LogTemp, Warning, TEXT("Changing State"));

	if(Enemy)
	{
		if(!Enemy->bUseControllerRotationYaw)
		Enemy->bUseControllerRotationYaw = true;
	}
	if (CurrentState) {
		CurrentState->OnEnterState();
	}
}


void AAIEnemy::SetStates(EEnemyType EnemyType)
{
	switch(EnemyType)
	{
	case EEnemyType::ogre:
	case EEnemyType::skeleton:
	case EEnemyType::knight:
		AttackState = MakeUnique<AIState_Attack>();
		DefenseState = MakeUnique<AIState_Defense>();
		break;
	case EEnemyType::imp:
		AttackState = MakeUnique<AIState_Attack>();
		DefenseState = MakeUnique<AIState_Imp_Defense>();
		break;

	case EEnemyType::mage:
		AttackState = MakeUnique<AIState_Range_Attack_Base>();
		DefenseState = MakeUnique<AIState_Range_Defense>();
		break;
	case EEnemyType::necroboss:
		AttackState = MakeUnique<AIState_Attack_Boss_Necro>();
		DefenseState = MakeUnique<AIState_Range_Defense>();
		break;
	case EEnemyType::monstrosityboss:
		AttackState = MakeUnique<AIState_Attack_Mostrosity>();
		DefenseState = MakeUnique<AIState_Defense>();
		break;
	case EEnemyType::salamanderboss:
		AttackState = MakeUnique<AIState_Salamander_Attack>();
		DefenseState = MakeUnique<AIState_Defense>();
		break;
	case EEnemyType::none:
		break;
	default:
		break;
	}
}

bool AAIEnemy::bStatesReady() const
{
	return AttackState && DefenseState && DeadState && IdleState;
}

void AAIEnemy::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	Enemy->bMovementCompleted = true;

	if (!GetWorldTimerManager().IsTimerActive(FResetMovementCompleted)) {
		TWeakObjectPtr<AEnemy>SoftEnemy = Enemy;
		GetWorldTimerManager().SetTimer(FResetMovementCompleted, [SoftEnemy]()
			{
				if (!SoftEnemy.IsValid()) return;

				if (AEnemy* localEnemy = SoftEnemy.Get()) {

						localEnemy->bWaitCompleted = true;
					
				}
			}, 2.f, false);
	}

}

void AAIEnemy::SetAIEnabled(bool bEnabled)
{
	CurrentState->SetEnabled(bEnabled);
}

void AAIEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	GetWorld()->GetTimerManager().ClearTimer(FWanderTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FResetMovementCompleted);
	GetWorld()->GetTimerManager().ClearTimer(FBeginPlayTimer);

	UnPossess();
	Destroy();
}

void AAIEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		Player = GI->localPlayer;
		if (Player && bStatesReady())
		{

			DeadState->PlayerWeakPtr = IdleState->PlayerWeakPtr = DefenseState->PlayerWeakPtr = AttackState->PlayerWeakPtr = Player;

		}
		else {

			GetWorldTimerManager().SetTimer(FBeginPlayTimer, [this]()
				{
					if (!bStatesReady()) return;

					if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
					{
						Player = GI->localPlayer;
						if (Player)
						{
							DeadState->PlayerWeakPtr = IdleState->PlayerWeakPtr = DefenseState->PlayerWeakPtr = AttackState->PlayerWeakPtr = Player;

							if (GetWorldTimerManager().IsTimerActive(FBeginPlayTimer))
							{
								GetWorldTimerManager().ClearTimer(FBeginPlayTimer);
							}
						}
					}

				}, 0.5, true);
		}
	}

}

void AAIEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsValid(Enemy)) return;

	if(CurrentState)
	{
		CurrentState->TickState(DeltaSeconds);
	}

	if(features.IsValidIndex(0) && features[0] != CurrentState->GetPlayerDistance())
	{
		features[0] = CurrentState->GetPlayerDistance();
	}

	if(PCS  && features.IsValidIndex(1) && features[1] != PCS->playerStats.currentHealth)
	{
		features[1] = PCS->playerStats.currentHealth;
	}

		if (Player) {
			if (Player->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {

				if (features.IsValidIndex(5))
					features[5] = Player->GetPlayerMontageTime();
			}

			if (features.IsValidIndex(4))
				features[4] = (Player->GetCurrentState() == PlayerStates::ATTACKING ? 2.0f : -2.0f);
		}
	

	
		if(features.IsValidIndex(2) && features[2] != Enemy->Health)
		{
			features[2] = Enemy->Health;
		}
		if(features.IsValidIndex(3) &&  features[3] != Enemy->GetStamina())
		{
			features[3] = Enemy->GetStamina();
		}

		if (AttackState && DefenseState) {

			float result = (AttackState->IsPlayerFacingAway(Enemy) || DefenseState->IsPlayerFacingAway(Enemy)) ? 1.0f : 0.0f;


			if (features.IsValidIndex(6) && features[6] != result) {
				features[6] = result;
			}
		}

		if (features.IsValidIndex(0)) {
				UE_LOG(LogTemp, Warning, TEXT("Enemy %s: Player Distance: %f  STATE: %s"), *Enemy->GetName() , features[0],*UEnum::GetValueAsName(CurrentState->GetState()).ToString());
			
		}
	if (features.IsValidIndex(1))
		UE_LOG(LogTemp, Warning, TEXT("Player Health: %f"), features[1]);
	if (features.IsValidIndex(2))
		UE_LOG(LogTemp, Warning, TEXT("Enemy Health: %f"), features[2]);
	if (features.IsValidIndex(3))
		UE_LOG(LogTemp, Warning, TEXT("Enemy Stamina: %f"), features[3]);
	if (features.IsValidIndex(4))
		UE_LOG(LogTemp, Warning, TEXT("Player Current State: %f"), features[4]);
	if (features.IsValidIndex(5))
		UE_LOG(LogTemp, Warning, TEXT("Player Montage time: %f"), features[5]);
	if (features.IsValidIndex(6))
		UE_LOG(LogTemp, Warning, TEXT("Is player facing away?: %f"), features[6]);
	




}

void AAIEnemy::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TWeakObjectPtr<AEnemy> EnemyWeakPtr = Cast<AEnemy>(GetPawn());

	Enemy = EnemyWeakPtr.Get();

	 AttackState = nullptr;
	 DefenseState = nullptr;
	 IdleState = nullptr;
	 DeadState = nullptr;

	if (Enemy) {
		SetStates(Enemy->EnemyType);


		IdleState = MakeUnique<AIState_Idle>();
		DeadState = MakeUnique<AIState_Dead>();
		DeadState->AIEnemyController = IdleState->AIEnemyController = DefenseState->AIEnemyController = AttackState->AIEnemyController = this;
		DeadState->world = IdleState->world = DefenseState->world = AttackState->world = GetWorld();
	}


	GetWorldTimerManager().SetTimer(ReadyCheckHandle, [this,EnemyWeakPtr]() {

		if (!EnemyWeakPtr.IsValid()) return;

		if (!Player  || !AttackState || !DefenseState || !DeadState || !IdleState) return;

		AEnemy* LocalEnemy = EnemyWeakPtr.Get();

		if (Player) {

			PCS = Cast<APlayerCharacterState>(Player->GetPlayerState());
			if (CurrentState && LocalEnemy && PCS) {
				features.Add(CurrentState->GetPlayerDistance()); //0
				features.Add(PCS->playerStats.currentHealth); //1
				features.Add(LocalEnemy->Health); //2
				features.Add(LocalEnemy->GetStamina()); //3
				features.Add(Player->GetCurrentState() == PlayerStates::ATTACKING ? 2.0f : -2.0f); //4
				features.Add(Player->GetPlayerMontageTime()); //5
				features.Add(AttackState->IsPlayerFacingAway(LocalEnemy) || DefenseState->IsPlayerFacingAway(LocalEnemy) ? 1.0f : 0.0f); //6
				features.Add(LocalEnemy->isFriendlyInRange() && LocalEnemy->EnemyParameters.bCanBuff && LocalEnemy->EnemyParameters.ShouldBuff(LocalEnemy->RecentEnemies, LocalEnemy->EnemyParameters.BuffObject) ? 1.0 : 0.0f); //7 used for checking if AI should buff or go to near by enemies.
				features.Add(LocalEnemy->Passivity); //8 Passivity float
				features.Add(LocalEnemy->EnemyParameters.bHasLongRange); //9 do we have a long range attack?
				features.Add(LocalEnemy->AttackCount >= LocalEnemy->AttackMontage->GetNumSections() ? 1.f : 0.f);//10 should we wait?
				AttackState->DecisionTree->Root = AttackState->BuildDecisionTree(LocalEnemy);
				DefenseState->DecisionTree->Root = DefenseState->BuildDecisionTree(LocalEnemy);

				GetWorldTimerManager().ClearTimer(ReadyCheckHandle);
			}
		}



		ChangeAIState(IdleState.Get());

		}
		, 1.0f, true
	);



	GetWorldTimerManager().SetTimer(CheckForNearbyEnemies, [this,EnemyWeakPtr]()
		{

			if (!EnemyWeakPtr.IsValid())
			{
				return;
			}

			if (AEnemy* LocalEnemy = EnemyWeakPtr.Get()) {
				if (features.IsValidIndex(7)) {
					features[7] = LocalEnemy->isFriendlyInRange() && LocalEnemy->EnemyParameters.bCanBuff && LocalEnemy->EnemyParameters.ShouldBuff(LocalEnemy->RecentEnemies, LocalEnemy->EnemyParameters.BuffObject) ? 1.0 : 0.0f;
				}

				float currentRandomNum = FMath::FRandRange(0.0f, 1.0f);

				if (features.IsValidIndex(8))
				{
					features[8] = currentRandomNum;
					UE_LOG(LogTemp, Warning, TEXT("Enemy: %s , feature 8 : %f"), *LocalEnemy->GetName(), features[8]);

				}

				if (features.IsValidIndex(10)) {
					UE_LOG(LogTemp, Warning, TEXT("Enemy: %s : feature 10 : %f, AttackCount : %d"),
						*LocalEnemy->GetName(), features[10], LocalEnemy->AttackCount);

					if (!GetWorld()->GetTimerManager().IsTimerActive(FWanderTimerHandle) && features[10] == 1.f)
					{
						TWeakObjectPtr<AAIEnemy> SafeEnemyController = this;
						GetWorld()->GetTimerManager().SetTimer(FWanderTimerHandle, [SafeEnemyController]()
							{
								if (!SafeEnemyController.IsValid()) return;

								if (AAIEnemy* LocalEnemyController = SafeEnemyController.Get()) {
									if (LocalEnemyController->features.IsValidIndex(10)) {
										LocalEnemyController->features[10] = 0.f;
									}
								}
							}, 1.5f, false);
					}
				}
			}

		}, .3, true);
}
