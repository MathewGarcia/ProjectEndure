// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Infinite_Poise.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void UAbility_Infinite_Poise::execute_Implementation()
{
	Super::execute_Implementation();

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			if(NiagaraSystemToPlay)
			{
				player->PlayAnimMontage(MontageToPlay, 1.f);
			if(UNiagaraComponent*NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached( NiagaraSystemToPlay, player->GetMesh(), "spine_01",FVector::ZeroVector,FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true))
			{
				NC = NiagaraComp;
				NiagaraComp->SetWorldRotation(FRotator(0.f, 90.f, 0.f));
			}
			}
		}

	}

}

bool UAbility_Infinite_Poise::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && !PCS->GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost);
		}
	}
	return false;
}

void UAbility_Infinite_Poise::Logic()
{
	Super::Logic();


	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
			TWeakObjectPtr<APlayerCharacterState>SafePlayerState = PCS;
					//increase physical resistance by 20%
					float& PoiseRef = PCS->playerStats.currentPoise;
					float Original = PoiseRef;
					PoiseRef = 9999;

					FTimerHandle FDuration;
					PCS->GetWorld()->GetTimerManager().SetTimer(FDuration, [this, Original, SafePlayerState]()
						{
							if (!SafePlayerState.IsValid() || !NC.IsValid()) return;

						if(APlayerCharacterState*SafePCS = SafePlayerState.Get())
						{
							SafePCS->playerStats.currentPoise = Original;

						}

						if(UNiagaraComponent*NiagaraComponent = NC.Get())
						{
							NiagaraComponent->DestroyComponent();
						}

						}, Duration, false);
				

			
		
		PCS->GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);

	}
}
