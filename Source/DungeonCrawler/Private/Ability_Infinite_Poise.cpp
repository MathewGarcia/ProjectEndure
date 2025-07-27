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
			if (NiagaraSystemToPlay && MontageToPlay && player->GetMesh())
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
			UWorld* World = PCS->GetWorld();
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && World && !World->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost);
		}
	}
	return false;
}

void UAbility_Infinite_Poise::Logic()
{
	Super::Logic();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		UWorld* World = PCS->GetWorld();
		if (!World) return;
		TWeakObjectPtr<APlayerCharacterState>SafePlayerState = PCS;
		//increase physical resistance by 20%
		float& PoiseRef = PCS->playerStats.currentPoise;
		float Original = PoiseRef;
		PoiseRef = 9999;
		FTimerHandle FDuration;
		if (World->GetTimerManager().IsTimerActive(FDuration)) {
			World->GetTimerManager().ClearTimer(FDuration);
		}
		World->GetTimerManager().SetTimer(FDuration, [this, Original, SafePlayerState]
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
		if (World->GetTimerManager().IsTimerActive(FCooldown)) {
			World->GetTimerManager().ClearTimer(FCooldown);
		}
		World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
	}
}
