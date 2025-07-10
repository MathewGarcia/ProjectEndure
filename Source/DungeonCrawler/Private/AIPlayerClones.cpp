// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPlayerClones.h"

#include "MainGameInstance.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void AAIPlayerClones::Execute()
{
	if (!Player) return;

	if (APlayerCharacter* OwningPawn = Cast<APlayerCharacter>(GetPawn()))
	{

		FVector ToPlayer = (Player->GetActorLocation() - OwningPawn->GetActorLocation());
		float DistanceToPlayer = ToPlayer.Size();
		FVector DirectionToPlayer = ToPlayer.GetSafeNormal();

		if (DistanceToPlayer > Threshold)
		{
			FVector Offset = FVector::CrossProduct(DirectionToPlayer, FVector::UpVector) * (Index % 2 == 0 ? -200.f : 200.f);
			FVector Location = OwningPawn->GetActorLocation() + DirectionToPlayer * (DistanceToPlayer - Threshold / 2) + Offset;
			MoveToLocation(Location);
		}

		if (Player->GetCurrentState() == PlayerStates::ATTACKING)
		{
			OwningPawn->Attack();

		}
		if(Player->GetCurrentState() == PlayerStates::ABILITY)
		{
			OwningPawn->SetActorRotation(Player->GetActorRotation());

		}
	}
}

void AAIPlayerClones::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Execute();
	
}

void AAIPlayerClones::BeginPlay()
{
	Super::BeginPlay();

}

void AAIPlayerClones::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("AI PLAYER CLONE POSSESSED"));

	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (MGI->localPlayer)
		{
			Player = MGI->localPlayer;
		}
	}

	if(APlayerCharacterState*PCS = GetPlayerState<APlayerCharacterState>())
	{
		PCS->playerStats.totalMana = 99999.f;
		PCS->playerStats.currentMana = 99999.f;
	}
}

void AAIPlayerClones::UsePlayerAbility(const int& pressedAbility)
{
	if (!Player) return;

	if (APlayerCharacter* OwningPawn = Cast<APlayerCharacter>(GetPawn()))
	{
		if (APlayerCharacterState* OwningPCS = OwningPawn->GetPlayerCharacterState()) {
			if (OwningPCS->EquippedAbilities.IsValidIndex(pressedAbility)) {
				OwningPawn->RecentlyPressedAbility = pressedAbility;
				if (UAbility* Ability = OwningPCS->EquippedAbilities[pressedAbility])
				{

					UE_LOG(LogTemp, Warning, TEXT("Using ability, %d"), pressedAbility);
					UE_LOG(LogTemp, Warning, TEXT("Ability, %s, Type: %s"), *Ability->GetName(), *Ability->GetClass()->GetName());
					UE_LOG(LogTemp, Warning, TEXT("Ability Memory Address: %p"), Ability);


					Ability->execute_Implementation();
				}
			}
		}
	}


}
