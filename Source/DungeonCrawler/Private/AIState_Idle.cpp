// Fill out your copyright notice in the Description page of Project Settings.


#include "AIState_Idle.h"

#include "Enemy.h"
#include "AIEnemy.h"
#include "PlayerCharacter.h"
#include "RoamingSpawnPoint.h"
#include "Components/SplineComponent.h"

AIState_Idle::AIState_Idle()
{
}

void AIState_Idle::OnEnterState()
{
	AIStateBase::OnEnterState();

	if (AEnemy* Enemy = Cast<AEnemy>(AIEnemyController->GetPawn()))
	{
		switch (Enemy->EnemyState)
		{
		case EEnemyStates::idleAttack:
			Enemy->GetWorldTimerManager().SetTimer(FakeAttackTimer, [this, Enemy]()
				{
				
					Enemy->PlayAnimMontage(Enemy->AttackMontage, 1.f, "Attack1");

				}, 1.0f, true);
			break;

			
		default:break;
		}
	}
}

void AIState_Idle::OnExitState()
{
	AIStateBase::OnExitState();
}

void AIState_Idle::TickState(float DeltaTime)
{
	AIStateBase::TickState(DeltaTime);

	if (AIEnemyController)
	{
		if (world)
		{
	
				if (AEnemy* Enemy = Cast<AEnemy>(AIEnemyController->GetPawn()))
				{
					if (!PlayerWeakPtr.IsValid()) return;

					APlayerCharacter* player = PlayerWeakPtr.Get();

					if (Enemy->EnemyState == EEnemyStates::roaming && Enemy->bMovementCompleted && Enemy->bWaitCompleted)
					{
						MoveToNextPoint();
						
					}
					FVector ToPlayer = (player->GetActorLocation() - Enemy->GetActorLocation());
					FVector DirToPlayer = ToPlayer.GetSafeNormal();
					float result = FVector::DotProduct(Enemy->GetActorForwardVector(), DirToPlayer);
					float threshold = FMath::Cos(FMath::DegreesToRadians(45.f));
				
					//all check for regular distance.
					if (result > threshold && PlayerDistance <= 800.f)
					{
						FHitResult hit;
						FVector Start;
						FRotator Rotation;
						Enemy->GetActorEyesViewPoint(Start,Rotation);
						FVector EndPoint = Start + (DirToPlayer * 1000);
						FCollisionQueryParams params;
						params.AddIgnoredActor(Enemy);
						DrawDebugLine(world, Start, EndPoint * 1000, FColor::Red, false, 1);
						if(world->LineTraceSingleByChannel(hit,Start,EndPoint,ECC_Pawn,params))
						{
							if(Enemy->GetWorldTimerManager().IsTimerActive(FakeAttackTimer))
							{
								Enemy->GetWorldTimerManager().ClearTimer(FakeAttackTimer);
							}

							if (Enemy->EnemyState != EEnemyStates::none)
							{

								Enemy->EnemyState = EEnemyStates::none;
							}

							if(AIEnemyController->IsFollowingAPath())
							{
								AIEnemyController->StopMovement();
							}
							AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get());

						}
					}
				}else
				{
					UE_LOG(LogTemp, Warning, TEXT("Enemy failed"));
				}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AIEnemyController failed"));
	}
}

float AIState_Idle::CalculateOptions()
{
	return AIStateBase::CalculateOptions();
}

EAIStates AIState_Idle::GetState()
{
	return EAIStates::idle;
}

void AIState_Idle::ExecuteOption(float weight, AEnemy* Enemy)
{
	AIStateBase::ExecuteOption(weight, Enemy);
}

TUniquePtr<AIDecisionTreeNode> AIState_Idle::BuildDecisionTree(AEnemy* Enemy)
{
	return AIStateBase::BuildDecisionTree(Enemy);
}

void AIState_Idle::MoveToNextPoint()
{

		// fetch the spline
		if (AEnemy* Enemy = Cast<AEnemy>(AIEnemyController->GetPawn()))
		{
			if (AIEnemyController->IsFollowingAPath() && !Enemy->bMovementCompleted && !Enemy->bWaitCompleted) return;

			if (ARoamingSpawnPoint* RoamingSpawnPoint = Cast<ARoamingSpawnPoint>(Enemy->GetOwner()))
			{

				if (!RoamingSpawnPoint->IsProcedural) {
					if (USplineComponent* SplineComponent = RoamingSpawnPoint->Spline)
					{
						// pick next index
						int32 NumSplinePoints = SplineComponent->GetNumberOfSplinePoints();
						CurrentSplinePoint = (CurrentSplinePoint + 1) % NumSplinePoints;

						// store the location
						NextLocation = SplineComponent->GetLocationAtSplinePoint(
							CurrentSplinePoint,
							ESplineCoordinateSpace::World
						);

						// now we actually tell the AI to move
						Enemy->bMovementCompleted = false;
						Enemy->bWaitCompleted = false;
						Result = AIEnemyController->MoveToLocation(NextLocation, 5.f);
					}
				}
				else
				{
					Enemy->bMovementCompleted = false;
					Enemy->bWaitCompleted = false;

					FVector Location = Enemy->GetActorLocation() + (FVector(FMath::RandRange(-1, 1), FMath::RandRange(-1, 1), 0)* 1000.f);
					Result = AIEnemyController->MoveToLocation(Location, 5.f);
				}
			}
		}
}

